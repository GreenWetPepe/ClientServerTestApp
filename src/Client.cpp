#include <iostream>
#include <boost/asio.hpp>

#include "Common.hpp"
#include "json.hpp"

using boost::asio::ip::tcp;

// Отправка сообщения на сервер по шаблону.
void SendMessage(
    tcp::socket& aSocket,
    const std::string& aLogin,
    const std::string& aRequestType,
    const std::string& aMessage)
{
    nlohmann::json req;
    req["UserLogin"] = aLogin;
    req["ReqType"] = aRequestType;
    req["Message"] = aMessage;

    std::string request = req.dump();
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()));
}

// Возвращает строку с ответом сервера на последний запрос.
std::string ReadMessage(tcp::socket& aSocket)
{
    boost::asio::streambuf b;
    boost::asio::read_until(aSocket, b, "\0");
    std::istream is(&b);
    std::string line(std::istreambuf_iterator<char>(is), {});
    return line;
}

std::string LoginEntry(tcp::socket& aSocket, const std::string& reqType)
{
    std::string login;
    std::string response;
    while (true)
    {
        std::cout << "Введите ваш логин (Если желаете прервать операцию, введите \"exit\") \n";
        std::cin >> login;
        if (login.empty()) continue;
        if (login == "exit") return "";

        SendMessage(aSocket, "0", Requests::CHECK_LOGIN, login);
        response = ReadMessage(aSocket);

        if (reqType == Requests::REGISTRATION)
        {
            if (response == ServerResponses::LOGIN_FREE) return login;
            std::cout << "Логин занят\n";
        }
        else if (reqType == Requests::LOG_IN)
        {
            if (response == ServerResponses::LOGIN_TAKEN) return login;
            std::cout << "Логин не найден\n";
        }
    }
    return "";
}

// "Создаём" пользователя, получаем его ID.
std::string ProcessRegistration(tcp::socket& aSocket)
{
    std::string login = LoginEntry(aSocket, Requests::REGISTRATION);
    if (login.empty()) return "";

    std::string password;
    std::cout << "Введите ваш пароль\n";
    std::cin >> password;
    if (password.empty()) return "";

    std::string message = login + " " + password;
    SendMessage(aSocket, "0", Requests::REGISTRATION, message);
    std::string response = ReadMessage(aSocket);
    if (response == ServerResponses::LOG_IN_COMPLETE)
    {
        return login;
    }
    return "";
}

std::string ProcessLogIn(tcp::socket& aSocket)
{
    std::string login = LoginEntry(aSocket, Requests::LOG_IN);
    if (login.empty()) return "";

    std::string password;
    std::cout << "Введите ваш пароль\n";
    std::cin >> password;
    SendMessage(aSocket, login, Requests::LOG_IN, password);
    std::string response = ReadMessage(aSocket);
    if (response == ServerResponses::LOG_IN_COMPLETE)
    {
        return login;
    }
    std::cout << "Неверный пароль\n";
    return "";
}

std::string FormDealRequest(int dealType)
{
    if (dealType != DealType::BUY && dealType != DealType::SELL) return "";

    std::string message = std::to_string(dealType) + " ";

    std::cout << "Начат процесс формирования заявки. Для отмены введите 0 или отрицательное число\n";

    std::cout << "Выберите кол-во долларов, которое желаете ";
    if (dealType == DealType::BUY) std::cout << "купить: ";
    else if (dealType == DealType::SELL) std::cout << "продать: ";

    std::string ans;
    std::cin >> ans;

    unsigned int requiredQuantity;
    try
    {
        requiredQuantity = std::stoi(ans);
    }
    catch(const std::exception& e)
    {
        std::cout << "Введите целое число\n";
        return "";
    }
    
    if (requiredQuantity <= 0) return "";

    message += std::to_string(requiredQuantity) + " ";

    std::cout << "Выберите кол-во оплаты за единицу: ";

    std::cin >> ans;
    unsigned int requiredPayment;
    try
    {
        requiredPayment = std::stoi(ans);
    }
    catch(const std::exception& e)
    {
        std::cout << "Введите целое число\n";
        return "";
    }
    if (requiredPayment <= 0) return "";

    message += std::to_string(requiredPayment) + " ";

    return message;
}

int main()
{
    try
    {
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "127.0.0.1", std::to_string(port));
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        s.connect(*iterator);

        int clienState = ClientStates::NOT_REGISTERED;
        std::string userLogin;
        while (true)
        {
            if (clienState == ClientStates::NOT_REGISTERED)
            {
                std::cout << "Menu:\n"
                         "1) Зарегистрироваться\n"
                         "2) Войти\n"
                         "3) Выйти\n"
                         << std::endl;
                int userChoice;
                std::cin >> userChoice;

                switch (userChoice)
                {
                case 1:
                    userLogin = ProcessRegistration(s);
                    if (!userLogin.empty())
                    {
                        std::cout << "Регистрация прошла успешно!\n"
                                    "Добро пожаловать!\n";
                        clienState = ClientStates::MAIN;
                    }
                    break;
                case 2:
                    userLogin = ProcessLogIn(s);
                    if (!userLogin.empty())
                    {
                        std::cout << "Вход в систему прошёл успешно!\n"
                                    "Добро пожаловать!\n";
                        clienState = ClientStates::MAIN;
                    }
                    break;
                case 3:
                    exit(0);
                    break;
                default:
                    std::cout << "Unknown menu option\n" << std::endl;
                    break;
                }
            }
            else if (clienState == ClientStates::MAIN)
            {
                SendMessage(s, userLogin, Requests::CHECK_NOTIFICATION, "");
                std::string response = ReadMessage(s);
                if (response != ServerResponses::EMPTY_STRING) std::cout << "\nУведомления\n" << response << "\n";

                std::cout << "Menu:\n"
                            "1) Купить валюту\n"
                            "2) Продать валюту\n"
                            "3) Просмотреть профиль\n"
                            "4) Просмотреть активные предложения\n"
                            "5) Просмотреть историю транзакций\n"
                            "6) Просмотреть личные предложения\n"
                            "7) Выйти\n"
                            << std::endl;

                short menu_option_num;
                std::cin >> menu_option_num;
                switch (menu_option_num)
                {
                    case 1:
                    {
                        std::string message = FormDealRequest(DealType::BUY);
                        if (message.empty()) 
                        {
                            std::cout << "Прекращение формирования заявки на покупку\n";
                            break;
                        }
                        SendMessage(s, userLogin, Requests::DEAL, message);
                        std::cout << ReadMessage(s);
                        break;
                    }
                    case 2:
                    {
                        std::string message = FormDealRequest(DealType::SELL);
                        if (message.empty()) 
                        {
                            std::cout << "Прекращение формирования заявки на продажу\n";
                            break;
                        }
                        SendMessage(s, userLogin, Requests::DEAL, message);
                        std::cout << ReadMessage(s);
                        break;
                    }
                    case 3:
                        SendMessage(s, userLogin, Requests::PROFILE, "");
                        std::cout << ReadMessage(s);
                        break;
                    case 4:
                    {
                        SendMessage(s, userLogin, Requests::VIEW_DEALS, "");
                        response = ReadMessage(s);
                        std::cout << "Активные предложения\n";
                        if (response != ServerResponses::EMPTY_STRING) std::cout << response << "\n";
                        else std::cout << "Отсутствуют\n";
                        break;
                    }
                    case 5:
                    {
                        SendMessage(s, userLogin, Requests::VIEW_TRANSACTIONS, "");
                        response = ReadMessage(s);
                        std::cout << "Транзакции\n";
                        if (response != ServerResponses::EMPTY_STRING) std::cout << response << "\n";
                        else std::cout << "Отсутствуют\n";
                        break;
                    }
                    case 6:
                    {
                        SendMessage(s, userLogin, Requests::VIEW_PERSONAL_DEALS, "");
                        response = ReadMessage(s);
                        std::cout << "Ваши предложения\n";
                        if (response != ServerResponses::EMPTY_STRING) std::cout << response << "\n";
                        else std::cout << "Отсутствуют\n";
                        break;
                    }
                    case 7:
                    {
                        exit(0);
                        break;
                    }
                    default:
                    {
                        std::cout << "Unknown menu option\n" << std::endl;
                        break;
                    }
                }
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
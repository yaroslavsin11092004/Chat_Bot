## Комментарии к проекту чат-бота на Spacy и TextBlob и Rasa
### О проекте
1. Чат-бот, опирающийся на фреймворки ***SpaCy*** и ***TextBlob*** является простым клиент-приложением для платформы Telegramm, 
со встроенным виртуальным окружением языка ***Python 3.10***. Основной интерфейс написан на языке ***C++*** с использованием
библиотеки ***Boost*** для асинхронных запросов и работы с ***TCP/IP*** и ***HTTP*** протоколами. Для возможности защищенного соедиения
была использована криптографическая библиотека ***OpenSSL***, обернутая в ***Boost***.
2. Чат-бот, опирающийся на фреймворк ***Rasa*** состоит из ***Rasa-сервера***, обрабатывающего пользовательские сообщения, интерфейса, написанного
на языке ***C++***, позволяющего отправить результат работы ***Rasa-сервера*** на сервер ***Telegramm***, а также отвечающего за весь
основной функционал, такой как:
* определение погоды
* логирование
* определение текущего времени и даты
* работы с базой данных MS Access Database
* рассчет математических выражений.
Для его реализации были использованы все библиотеки, перечисленные в пункте 1. 
### О работе с проектом
* В текущем состоянии он не пригоден для сборки и компиляции на пользовательском утройстве в силу отсутствия скриптов сборки и инструментов 
***CMake***. Кроме того, для расчета математических выражений был использован парсер из написанной мной библиотеки, которой нет в открытом
доступе. В директории ***./Chat_Bot/x64/Release*** находится скомпилированная версия проекта для 64-битных систем. Для того, чтобы ее запустить,
требуется заполнить json файл ***config.json***, расположенный в каталоге ***./Chat_Bot*** с обязательным указанием ***каталога, в котором расположена папка с проектом*** 
(***"prefix_path"***), ***домашней директории интерпретатора python 3.10*** (***python_home***), ***токена api.telegramm***(***telegramm_api_token***) и
***токена api.openwethermap***(***"open_wether_api_token"***). 
* Перед использованием фреймворка ***Rasa*** требуется запустить Rasa-сервер с помощью скрипта для Windows Power Shell ***rasa_start.ps1***, находящегося в 
директории ***./Chat_Bot/TelegrammBot***. 
* Для запуска exe-файла перейти в каталог с exe-файлом и ввести в Power Shell команду ***start ./TelegrammBot.exe **path_to_config*****, где **path_to_config** - 
путь до файла **config.json**.
* Для работы с ботом запустить скрипт ***py_install.ps1*** для установки библиотек ***Spacy***, ***TextBlob***, и ***googletrans***
* В начале работе приложения можно выбрать фреймворк, на котором будет строиться работа чат-бота
* Команды для работы с консолью:
  * **clear** - очищает консоль
  * **exit** - выход из приложения
  * **get-log** - вывод истории дилога с ботом (после ввода этой команды необходимо ввести ***user_name*** аккаунта ползователя Telegramm)
#### Некоторые примеры работы программы:
* Выбор фреймворка:
    ![Choice Framework](/ScreenShots/ChoiceFramework.png)
* Пример вывода в консоль диалога с пользователем:
    ![Logging Dialog](/ScreenShots/LogDialog.png)
* Пример диалога в Telegramm
    ![Telegramm Dialog](/ScreenShots/Example.png)
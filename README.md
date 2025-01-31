# komunikator

#Instrukcja uruchomienia

Wymagane biblioteki:
+ nlohmann/json
+ wxWidgets (conajmniej 3.2.4) - dla klienta, pobrane z: https://www.wxwidgets.org/downloads/
+ sqlite3
+ inne biblioteki od których zależą powyższe 

Uruchamianie po pobraniu:

Klient:
+ klient/build/
+ usunąć CMakeCashe.txt
+ cmake ..
+ make
+ ./bin/Klient

Serwer:
+ serwer/build/
+ usunąć CMakeCashe.txt
+ cmake ..
+ make
+ ./bin/Serwer

Bazy danych dostepne w build/resource/resource

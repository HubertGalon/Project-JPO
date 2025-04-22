# Monitor Jakości Powietrza

Aplikacja do monitorowania jakości powietrza w Polsce wykorzystująca dane z API Głównego Inspektoratu Ochrony Środowiska.

## Opis projektu

Aplikacja umożliwia:
- Pobieranie aktualnych danych o stacjach pomiarowych, czujnikach i pomiarach
- Wyszukiwanie stacji po mieście lub lokalizacji geograficznej
- Wyświetlanie danych pomiarowych w formie wykresów
- Analizowanie trendów w danych
- Pracę w trybie offline z wykorzystaniem zapisanych danych

## Wymagania

- Qt 5.15 lub nowszy
- Kompilator C++ zgodny ze standardem C++14 lub nowszym
- Połączenie internetowe (opcjonalne, aplikacja może działać offline)
- Uprawnienia do zapisu w katalogu aplikacji (dla lokalnej bazy danych)

## Struktura projektu

Projekt podzielony jest na następujące moduły:
- API Client: Komunikacja z API GIOŚ
- Database Handler: Zarządzanie lokalną bazą danych
- Data Manager: Centralny komponent zarządzający przepływem danych
- UI Components: Widgety interfejsu użytkownika
- Data Analysis: Narzędzia do analizy danych pomiarowych
- Tests: Testy jednostkowe

## Uruchomienie aplikacji

### Kompilacja

1. Otwórz projekt w Qt Creator:
   ```
   File -> Open File or Project -> wybierz plik .pro
   ```

2. Skonfiguruj projekt wybierając odpowiedni zestaw narzędzi (kit)

3. Kliknij przycisk "Run" (Ctrl+R) lub wybierz:
   ```
   Build -> Build Project "Monitor Jakości Powietrza"
   ```

4. Po pomyślnej kompilacji, uruchom aplikację przyciskiem "Run"

### Uruchomienie z wiersza poleceń

1. Przejdź do katalogu z plikiem wykonywalnym:
   ```
   cd ścieżka/do/katalogu/build
   ```

2. Uruchom aplikację:
   ```
   ./MonitorJakosciPowietrza
   ```

## Uruchomienie testów jednostkowych

### Kompilacja testów

1. Otwórz plik projektu testów w Qt Creator:
   ```
   File -> Open File or Project -> wybierz plik tests/tests.pro
   ```

2. Skonfiguruj projekt testów

3. Skompiluj testy:
   ```
   Build -> Build Project "tests"
   ```

### Uruchamianie poszczególnych testów

1. Przejdź do katalogu z danym testem:
   ```
   cd ścieżka/do/katalogu/tests/MeasurementTest
   ```

2. Uruchom test:
   ```
   ./MeasurementTest
   ```

Podobnie dla pozostałych testów:
- DataAnalyzerTest
- DatabaseHandlerTest

### Uruchamianie testów z Qt Creator

1. Otwórz widok "Projects" w Qt Creator
2. Przejdź do zakładki "Run"
3. Kliknij przycisk "Run" aby uruchomić aktualnie aktywny test
4. Wyniki testu pojawią się w oknie "Application Output"

## Konfiguracja aplikacji

Aplikacja automatycznie tworzy katalog danych w lokalizacji standardowej dla systemu operacyjnego (np. `%APPDATA%\Local\AirQualityMonitor` w Windows).

### Tryb offline

Aplikacja wykrywa dostępność połączenia internetowego i umożliwia przełączanie między trybem online i offline:
- W przypadku braku połączenia internetowego, aplikacja automatycznie przełącza się w tryb offline
- Tryb można przełączyć ręcznie za pomocą przycisku w pasku statusu
- W trybie offline używane są dane zapisane w lokalnej bazie

## Generowanie dokumentacji

Do wygenerowania dokumentacji używamy systemu Doxygen:

1. Zainstaluj Doxygen: https://www.doxygen.nl/download.html
2. Uruchom program Doxywizard
3. W zakładce "Project" ustaw:
   - Project name: "Monitor Jakości Powietrza"
   - Source code directory: ścieżka do katalogu projektu
   - Destination directory: ścieżka/do/projektu/docs
4. W zakładce "Mode" wybierz "All Entities" i zaznacz "Optimize for C++ output"
5. W zakładce "Output" wybierz format wyjściowy (np. HTML)
6. Włącz opcję "Scan recursively"
7. Kliknij "Run doxygen" w zakładce "Run"

Dokumentacja zostanie wygenerowana w katalogu `docs/html`. Otwórz plik `index.html` w przeglądarce, aby ją zobaczyć.

## Autor

Autor: Hubert Galon

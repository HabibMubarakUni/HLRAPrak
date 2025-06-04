#include <string>
#include <vector>
#include <atomic>
#include <iostream>
#include <fstream>
#include <thread>
#include <sstream>
#include <chrono>
#include "includes/cli.h"
#include "includes/World.h"
#include <limits> // Für std::numeric_limits


void clearScreen() {
    std::cout << "\033[2J\033[H";  // ANSI-Escape-Sequenz zum Löschen des Bildschirms und Zurücksetzen des Cursors
}

CommandLineInterface::CommandLineInterface() {}

void CommandLineInterface::menu() {
    int width = 10, height = 10;
    bool showOutput = false;
    bool stopOnStable = false;  // Option für automatische Stopp bei stabiler Welt
    int delay = 300;  // Default Delay für Simulation
    World world(height, width);
    bool isWorldCreated = false;

    while (true) {
        if (!isWorldCreated) {
            clearScreen();
            std::cout << "== Hauptmenü ===\n";
            std::cout << "1. Welt erstellen\n";
            std::cout << "2. Welt laden\n";
            std::cout << "3. Beenden\n";
            std::cout << ">> ";
            int choice;
            std::cin >> choice;
            clearScreen();
            if (std::cin.fail()) {
                std::cin.clear(); std::cin.ignore(10000, '\n');
                std::cout << "Ungültige Eingabe.\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(700));
                continue;
            }

            switch (choice) {
                case 1: {
                    int height = 0, width = 0;

                    // Höhe abfragen mit Validierung
                    while (true) {
                        std::cout << "Geben Sie die Höhe der Welt ein: ";
                        std::cin >> height;

                        if (std::cin.fail() || height <= 0) {
                            std::cin.clear(); // Fehler-Status zurücksetzen
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Rest der Zeile ignorieren
                            std::cout << "Ungültige Eingabe. Bitte geben Sie eine positive ganze Zahl ein.\n";
                            std::this_thread::sleep_for(std::chrono::milliseconds(700));
                            clearScreen();
                        } else {
                            break;
                        }
                    }
                    clearScreen();

                    // Breite abfragen mit Validierung
                    while (true) {
                        std::cout << "Geben Sie die Breite der Welt ein: ";
                        std::cin >> width;

                        if (std::cin.fail() || width <= 0) {
                            std::cin.clear(); // Fehler-Status zurücksetzen
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Rest der Zeile ignorieren
                            std::cout << "Ungültige Eingabe. Bitte geben Sie eine positive ganze Zahl ein.\n";
                            std::this_thread::sleep_for(std::chrono::milliseconds(700));
                            clearScreen();
                        } else {
                            break;
                        }
                    }
                    clearScreen();

                    world = World(height, width);
                    isWorldCreated = true;

                    if (showOutput) world.print();
                    break;
                }
                case 2: {
                    std::cout << "Geben Sie den Dateinamen zum Laden ein: ";
                    std::string file_name;
                    std::cin >> file_name;
                    file_name += ".txt";
                    if (world.load(file_name)) {
                        isWorldCreated = true;
                        clearScreen();
                        if (showOutput) world.print();
                    } else {
                        std::cout << "Fehler beim Laden der Welt.\n";
                    }
                    break;
                }
                case 3:
                    {
                        char confirm;
                        std::cout << "Sind Sie sicher, dass Sie das Programm beenden möchten? (j/n): ";
                        std::cin >> confirm;
                        if (confirm == 'j' || confirm == 'J') {
                            std::cout << "Programm beendet.\n";
                            return;
                        } else {
                            std::cout << "Abbruch des Beendens.\n";
                            break;
                        }
                    }
                    return;
                default:
                    std::cout << "Ungültige Auswahl.\n";
            }
        } else {
            clearScreen();
            if (showOutput) world.print();
            std::cout << "\n=== Optionen für die Welt ===\n";
            std::cout << "1. Muster hinzufügen\n";
            std::cout << "2. Simulation starten\n";
            std::cout << "3. Zellenabfrage/-änderung\n";
            std::cout << "4. Einstellungen\n";
            std::cout << "5. Speichern\n";
            std::cout << "6. Zurück\n";
            std::cout << "7. Beenden\n";
            std::cout << ">> ";

            int choice;
            std::cin >> choice;
            if (std::cin.fail()) {
                std::cin.clear(); // Fehlerzustand zurücksetzen
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Eingabepuffer leeren
                std::cerr << "Ungültige Eingabe. Bitte nur Zahlen eingeben.\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(700));
                continue; // zurück zum Menü
            }
            clearScreen();
            if (showOutput) world.print();

            switch (choice) {
                case 1: {
                    std::cout << "Muster hinzufügen:\n";
                    std::cout << "1. Glider einfügen\n";
                    std::cout << "2. Toad einfügen\n";
                    std::cout << "3. Beacon einfügen\n";
                    std::cout << "4. Methuselah einfügen\n";
                    std::cout << "5. Zufällige Muster einfügen\n";
                    std::cout << "6. Zurück\n";
                    std::cout << ">> ";
                    int patternChoice;
                    std::cin >> patternChoice;
                    clearScreen();
                    if (showOutput) world.print();

                    int x = 0, y = 0; // Initialisieren der Variablen für Koordinaten

                    if (patternChoice >= 1 && patternChoice <= 4) {
                        std::cout << "Geben Sie die X-Koordinate ein: ";
                        std::cin >> x;
                        std::cout << "Geben Sie die Y-Koordinate ein: ";
                        std::cin >> y;
                        clearScreen();
                    }

                    switch (patternChoice) {
                        case 1: world.add_glider(x, y); break;
                        case 2: world.add_toad(x, y); break;
                        case 3: world.add_beacon(x, y); break;
                        case 4: world.add_methuselah(x, y); break;
                        case 5: {
                            int n;
                            std::cout << "Wie viele Muster möchten Sie zufällig einfügen? ";
                            std::cin >> n;
                            clearScreen();
                            world.add_random_patterns(n);
                            break;
                        }
                    }
                    break;
                }
                case 2: {
                    clearScreen();
                    if (showOutput) world.print();
                    std::cout << "Simulation starten:\n";
                    std::cout << "1. Unendlich laufen lassen (mit Stop durch Taste oder Stabilität)\n";
                    std::cout << "2. Für eine bestimmte Anzahl an Generationen\n";
                    std::cout << ">> ";
                    int simMode;
                    std::cin >> simMode;

                    int maxGenerations = -1;
                    if (simMode == 2) {
                        clearScreen();
                        if (showOutput) world.print();
                        std::cout << "Wie viele Generationen sollen simuliert werden? ";
                        std::cin >> maxGenerations;
                    }

                    clearScreen();
                    std::cout << "Simulation wird gestartet... (Drücken Sie 'q' zum Stoppen)\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

                    std::atomic<bool> stop(false);
                    std::thread inputThread;

                    if (simMode == 1) {
                        inputThread = std::thread([&stop]() {
                            while (!stop) {
                                if (std::cin.peek() != EOF) {  // peek, um nicht zu blockieren
                                    char input;
                                    std::cin >> input;
                                    if (input == 'q' || input == 'Q') {
                                        stop = true;
                                        break;
                                    }
                                }
                                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                            }
                        });
                    }



                    int generations = 0;

                    auto startTime = std::chrono::high_resolution_clock::now();

                    while (!stop) {
                        if (stopOnStable && world.is_stable()) {
                            stop = true;
                            clearScreen();
                            if (showOutput) world.print();
                            std::cout << "Die Welt ist stabil. Simulation wurde automatisch beendet.\n";
                            break;
                        }

                        if (simMode == 2 && generations >= maxGenerations) {
                            stop = true;
                            break;
                        }
                        world.evolve();
                        generations++;

                        if (showOutput) {
                            clearScreen();
                            world.print();
                            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
                        }

                    }

                    if (inputThread.joinable()) {
                        stop = true;  // nur zur Sicherheit – dürfte bereits true sein
                        inputThread.join(); // Eingabe-Thread korrekt beenden
                    }

                    auto endTime = std::chrono::high_resolution_clock::now();
                    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

                    if (durationMs <= 0) {
                        auto durationUs = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
                        std::cout << "Simulation für " << generations << " Generationen.\n";
                        std::cout << "Gesamtdauer: " << durationUs << " Mikrosekunden.\n";
                    } else {
                        std::cout << "Simulation für " << generations << " Generationen.\n";
                        std::cout << "Gesamtdauer: " << durationMs << " Millisekunden.\n";
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                    break;
                }
                case 3: {
                    int subOption;
                    std::cout << "\nZellenabfrage und -änderung:" << std::endl;
                    std::cout << "1. Zellstatus abfragen (x y)" << std::endl;
                    std::cout << "2. Zellstatus abfragen (p)" << std::endl;
                    std::cout << "3. Zellstatus setzen (x y state)" << std::endl;
                    std::cout << "4. Zellstatus setzen (p state)" << std::endl;
                    std::cout << "5. Zurück" << std::endl;
                    std::cout << ">> ";
                    std::cin >> subOption;
                
                    switch (subOption) {
                        case 1: {
                            int x, y;
                            std::cout << "Geben Sie x und y ein: ";
                            std::cin >> x >> y;
                            int state = world.get_cell_state(x, y);
                            if (state != -1)
                                std::cout << "Zelle (" << x << ", " << y << ") ist " << (state ? "lebendig\n" : "tot\n");
                            std::this_thread::sleep_for(std::chrono::milliseconds(700));
                            break;
                        }
                        case 2: {
                            int p;
                            std::cout << "Geben Sie p ein: ";
                            std::cin >> p;
                            int state = world.get_cell_state(p);
                            if (state != -1)
                                std::cout << "Zelle an Position " << p << " ist " << (state ? "lebendig\n" : "tot\n");
                            std::this_thread::sleep_for(std::chrono::milliseconds(700));
                            break;
                        }
                        case 3: {
                            int x, y, s;
                            std::cout << "Geben Sie x, y und den Zustand (0/1) ein: ";
                            std::cin >> x >> y >> s;
                            bool state = world.set_cell_state(s, x, y);
                            if (state == true)
                                std::this_thread::sleep_for(std::chrono::milliseconds(700));
                            break;
                        }
                        case 4: {
                            int p, s;
                            std::cout << "Geben Sie p und den Zustand (0/1) ein: ";
                            std::cin >> p >> s;
                            bool state = world.set_cell_state(s, p);
                            if (state == true)
                                std::this_thread::sleep_for(std::chrono::milliseconds(700));
                            break;
                        }
                        case 5:
                            break;
                        default:
                            clearScreen();
                            if (showOutput) world.print();
                            std::cerr << "Ungültige Eingabe. Bitte nur Zahlen eingeben.\n";
                            std::this_thread::sleep_for(std::chrono::milliseconds(700));
                    }
                    break;
                }
                case 4: {
                    clearScreen();
                    if (showOutput) world.print();
                    std::cout << "\n=== Einstellungen ===\n";
                    std::cout << "1. Anzeige: " << (showOutput ? "ein" : "aus") << std::endl;
                    std::cout << "2. Delay ändern: " << delay << std::endl;
                    std::cout << "3. Stopp bei Stabilität: " << (stopOnStable ? "an" : "aus") << std::endl;
                    std::cout << "4. Zurück\n";
                    std::cout << ">> ";
                    
                    int settingsChoice;
                    std::cin >> settingsChoice;
                    if (std::cin.fail()) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cerr << "Ungültige Eingabe. Bitte nur ganze Zahlen eingeben.\n";
                        std::this_thread::sleep_for(std::chrono::milliseconds(700));
                        break;  // zurück zum Menü
                    }

                    clearScreen();
                    if (showOutput) world.print();

                    switch (settingsChoice) {
                        case 1:
                            showOutput = !showOutput;
                            std::cout << "Anzeige " << (showOutput ? "aktiviert" : "deaktiviert") << ".\n";
                            break;
                        case 2: {
                            std::cout << "Geben Sie das neue Delay (in ms) ein: ";
                            int newDelay;
                            std::cin >> newDelay;
                            if (std::cin.fail() || newDelay < 0) {
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                std::cerr << "Ungültiger Wert für Delay.\n";
                                std::this_thread::sleep_for(std::chrono::milliseconds(700));
                                break;
                            }
                            delay = newDelay;
                            std::cout << "Delay auf " << delay << " ms gesetzt.\n";
                            break;
                        }
                        case 3:
                            stopOnStable = !stopOnStable;
                            std::cout << "Automatischer Stopp bei Stabilität ist jetzt " << (stopOnStable ? "aktiviert" : "deaktiviert") << ".\n";
                            break;
                        case 4:
                            break;
                        default:
                            std::cerr << "Ungültige Auswahl. Bitte nur Zahlen von 1 bis 4 eingeben.\n";
                            std::this_thread::sleep_for(std::chrono::milliseconds(700));
                    }
                    break;
                }
                case 5: {
                    clearScreen();
                    std::cout << "Geben Sie den Dateinamen zum Speichern ein: ";
                    std::string file_name;
                    std::cin >> file_name;
                    world.save(file_name + ".txt");
                    std::cout << "Gespeichert als " << file_name << ".txt\n";
                    break;
                }
                
                case 6:{
                    isWorldCreated = false;
                    break;
                }

                case 7: {
                    char confirm;
                    std::cout << "Sind Sie sicher, dass Sie das Programm beenden möchten? (j/n): ";
                    std::cin >> confirm;
                    if (confirm == 'j' || confirm == 'J') {
                        std::cout << "Programm beendet.\n";
                        return;
                    } else {
                        std::cout << "Abbruch des Beendens.\n";
                        break;
                    }
                }
                
                default:
                    std::cerr << "Ungültige Eingabe. Bitte nur Zahlen eingeben.\n";
            }
        }
    }
}

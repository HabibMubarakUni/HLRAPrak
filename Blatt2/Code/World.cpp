#include "includes/World.h"     // Einbindung der Header-Datei mit der Klassendeklaration
#include <fstream>              // Für Dateioperationen (z.B. Datei schreiben oder lesen)     
#include <iostream>
#include <cstdlib> // für rand()
#include <ctime>   // für time()
#include <chrono>
#include <thread>
#include <filesystem>


// Konstruktor mit Höhe und Breite
World::World(int height, int width) {
    // Speichert die übergebenen Werte in den Membervariablen
    this->height = height;
    this->width = width;
    this->generation = 0;

    // Initialisiert das Grid mit Nullen (height x width)
    this->grid.resize(height);  // Anzahl der Zeilen

    for (int i = 0; i < height; ++i) {
        this->grid[i] = std::vector<int>(width, 0);  // Jede Zeile mit Nullen füllen
    }
}

// Konstruktor mit Dateiname – lädt eine Welt aus einer Datei
World::World(std::string file_name) {
    // Öffnet die Datei zum Lesen
    std::ifstream file(file_name);

    // Überprüft, ob die Datei erfolgreich geöffnet wurde
    if (!file.is_open()) {
        std::cerr << "Datei konnte nicht geöffnet werden: " << file_name << std::endl;
        exit(1); // Programm beenden bei Fehler
    }

    // Liest Höhe und Breite aus der Datei
    file >> this->height >> this->width;

    // Initialisiert das Grid mit den gelesenen Maßen und füllt es mit Nullen
    this->grid.resize(height);  // Jetzt existieren grid[0] bis grid[height-1]
    for (int i = 0; i < height; ++i) {
        this->grid[i] = std::vector<int>(width, 0);  // Jede Zeile mit Nullen füllen
    }
    
    // Liest die Werte für das Grid aus der Datei
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            file >> this->grid[i][j]; // nächste Element wird gelesen (Leerzeichen, Tabs und Zeilenumbrüche werden übersprungen)
        }
    }

    // Schließt die Datei nach dem Lesen
    file.close();
}

// Speichert die aktuelle Welt in eine Datei
void World::save(std::string file_name) {
    if (!std::filesystem::exists("save")) {
        std::filesystem::create_directory("save");
    }
    // Öffnet die Datei zum Schreiben (überschreibt bestehende Inhalte)
    std::ofstream file("save/" + file_name);

    // Überprüft, ob die Datei erfolgreich geöffnet wurde
    if (!file.is_open()) {
        std::cerr << "Datei konnte nicht zum Schreiben geöffnet werden: " << file_name << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return;
    }

    // Schreibt Höhe und Breite in die erste und zweite Zeile
    file << this->height << std::endl;
    file << this->width << std::endl;

    // Schreibt den Inhalt des Grids in die Datei
    for (int i = 0; i < this->height; ++i) {
        // Schleife über alle Spalten in der aktuellen Zeile
        for (int j = 0; j < this->width; ++j) {
            // Schreibt den aktuellen Wert der Zelle [i][j] in die Datei
            file << this->grid[i][j];

            // Fügt ein Leerzeichen als Trennzeichen hinzu, außer bei der letzten Spalte
            // Dadurch wird am Zeilenende kein zusätzliches Leerzeichen geschrieben
            if (j < this->width - 1) 
                file << " ";
        }
        // Nach jeder Zeile des Grids wird ein Zeilenumbruch in die Datei geschrieben
        // Damit beginnt die nächste Grid-Zeile in der nächsten Textzeile der Datei
        file << std::endl;
    }

    // Schließt die Datei nach dem Schreiben
    file.close();
}

void World::print() {
    std::cout << "Generation: " << generation << "\n";
    for (const auto& row : grid) { // auto: Der Compiler leitet selbst ab, welchen Typ row hat const (std::vector<int>&), basierend auf dem Typ von grid
        for (int cell : row) {
            if (cell) {
                std::cout << "\033[1m\033[32m\u2593\u2593\033[0m "; // grün
            } else {
                std::cout << "\033[1m\033[90m\u2591\u2591\033[0m "; // grau
            }
        }
        std::cout << "\n";
    }
}


int World::count_alive_neighbours(int x, int y) {
    int aliveNeighbours = 0;
    // jede Zelle hat 8 Nachbarn (auch die Ecken da Welt toroidal ist)
    // Schleife über die benachbarten Zellen
    // dx verändert die horizontale Position
    // dy verändert die vertikale Position

    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
  
        // Berechnung der Position der benachbarten Zelle unter Berücksichtigung von Randbedingungen
        int sx = (x + dx + this->width) % this->width; // Berechnet die neue Zeilenposition
        int sy = (y + dy + this->height) % this->height; // Berechnet die neue Spaltenposition
  
        // Die aktuelle Zelle selbst überspringen indem man die Indices überprüft
        if (dx != 0 || dy != 0) {
          aliveNeighbours += this->grid[sy][sx];
        }
      }
    }
  
    return aliveNeighbours;
  }
  
  
  

void World::evolve() {
    // Neues Grid für die nächste Generation
    std::vector<std::vector<int>> newGrid;
    newGrid.resize(this->height); // height Zeilen
    for (int i = 0; i < this->height; ++i) {
        newGrid[i] = std::vector<int>(this->width, 0);  // Jede Zeile mit Nullen füllen
    }

    // Gehe jede Zelle durch und wende die Spielregeln an
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            int aliveNeighbours = count_alive_neighbours(x, y);

            // Regel 1: Lebende Zelle mit weniger als 2 lebenden Nachbarn stirbt
            if (this->grid[y][x] == 1 && aliveNeighbours < 2) {
                newGrid[y][x] = 0;
            }
            // Regel 2: Lebende Zelle mit 2 oder 3 lebenden Nachbarn bleibt am Leben
            else if (this->grid[y][x] == 1 && (aliveNeighbours == 2 || aliveNeighbours == 3)) {
                newGrid[y][x] = 1;
            }
            // Regel 3: Lebende Zelle mit mehr als 3 lebenden Nachbarn stirbt
            else if (this->grid[y][x] == 1 && aliveNeighbours > 3) {
                newGrid[y][x] = 0;
            }
            // Regel 4: Tote Zelle mit genau 3 lebenden Nachbarn wird lebendig
            else if (this->grid[y][x] == 0 && aliveNeighbours == 3) {
                newGrid[y][x] = 1;
            }
        }
    }

    // Übernehme das neue Grid und erhöhe die Generation
    this->grid = newGrid;
    this->generation++;
}

bool World::is_equal(const std::vector<std::vector<int>>& grid_1,
    const std::vector<std::vector<int>>& grid_2) {
// vergleiche Zellen in beiden Grids
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // return false wenn mind. 1 unterschiedlich
            if (grid_1[i][j] != grid_2[i][j]) {
                return false;
            }
        }
    }
    // return true, wenn alle Zellen gleich
    return true;
}

bool World::is_stable() {
    World world_copy(*this);
    world_copy.evolve();
    if (!is_equal(grid, world_copy.grid)) {
      world_copy.evolve();
      if (!is_equal(grid, world_copy.grid)) {
        return false;
      }
    }
    return true;
  }

  

bool World::load(std::string file_name) {
    // Ordner "save/" sicherstellen
    std::filesystem::path save_dir("save");
    if (!std::filesystem::exists(save_dir)) {
        std::filesystem::create_directory(save_dir);
        std::cout << "Verzeichnis 'save/' wurde erstellt.\n";
    }

    std::ifstream file(save_dir / file_name); // sauberer mit filesystem::path

    if (!file.is_open()) {
        std::cerr << "Datei konnte nicht geöffnet werden: " << file_name << std::endl;
        std::cerr << "Aktueller Pfad: " << std::filesystem::current_path() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return false;
    }

    // Dateiinhalt lesen
    file >> this->height >> this->width;
    this->grid.resize(height);
    for (int i = 0; i < height; ++i)
        this->grid[i] = std::vector<int>(width, 0);

    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j)
            file >> this->grid[i][j];

    this->generation = 0;
    file.close();
    return true;
}



int World::get_cell_state(int x, int y) {
    // Return Zellstatus, falls Koordinaten gültig sind
    if (x >= 0 && x < width && y >= 0 && y < height) {
      return grid[y][x];
    }
    // Return -1 und gebe Error aus, wenn Koordinaten ungültig sind
    else {
      std::cerr << "ungültige Koordinaten: (" << x << ", " << y << ")" << std::endl;
      return -1;
    }
}
  
int World:: get_cell_state(int p) {
    // 2d Koordinaten mit einem Punkt und return Zellstatus (falls Punkt gültig)
    if (p >= 0 && p < this->height * this->width) {
      int y = p / this->width;
      int x = p % this->width;
      return grid[y][x];
    }
    // Return -1 und gebe Error aus, wenn Punkt ungültig
    else {
      std::cerr << "Punkt " << p << "außerhalb der Welt" << std::endl;
      return -1;
    }
}

bool World::set_cell_state(int state, int x, int y) {
    // Setze den Zellenstatus auf x, wenn Koordinaten gültig
    if (x >= 0 && x < width && y >= 0 && y < height) {
      this->grid[y][x] = state;
      return true;
    }
    // gibt Fehler aus, wenn Koordinaten ungültig
    else {
      std::cerr << "ungültige Koordinaten: (" << x << ", " << y << ")" << std::endl;
      return false;
    }
}
  
bool World::set_cell_state(int state, int p) {
    int cell_count = this->height * this->width;
    // Bestimmung von 2D-Koordinaten aus dem Punkt und setzen des Zellenstatus, wenn der Punkt gültig ist
    if (cell_count > p) {
      int y = p / this->width;
      int x = p % this->width;
      this->grid[y][x] = state;
      return true;
      // gebe Fehlermeldung aus, wenn Punkt ungültig
    } else {
      std::cerr << "Punkt " << p << "liegt außerhalb der Welt" << std::endl;
      return false;
    }
}

void World::add_glider(int x, int y) {
    // Füge glider Pattern ein, wenn Koordinaten innerhalb der Welt
    if (x >= 0 && y >= 0 && x < width && y < height) {
      this->grid[y][(x+1) % width] = 1;
      this->grid[(y+1) % height][(x+2) % width] = 1;
  
      this->grid[(y+2) % height][x] = 1;
      this->grid[(y+2) % height][(x+1) % width] = 1;
      this->grid[(y+2) % height][(x+2) % width] = 1;
    }
    // Fehlermeldung, wenn Koordinaten außerhalb der Welt
    else {
        std::cerr << "ungültige Koordinaten um Glider einzufügen" << std::endl;
    }
}

void World::add_toad(int x, int y) {
    // Füge toad Pattern ein, wenn Koordinaten innerhalb der Welt
    if (x >= 0 && y >= 0 && x < width && y < height) {
      this->grid[y][(x+2) % width] = 1;
      this->grid[(y+1) % height][x] = 1;
      this->grid[(y+2) % height][x] = 1;
      this->grid[(y+1) % height][(x+3) % width] = 1;
      this->grid[(y+2) % height][(x+3) % width] = 1;
      this->grid[(y+3) % height][(x+1) % width] = 1;
    }
    // Fehlermeldung, wenn Koordinaten außerhalb der Welt

    else {
        std::cerr << "ungültige Koordinaten um Toad einzufügen" << std::endl;
    }
}


void World::add_beacon(int x, int y) {
    if (x >= 0 && y >= 0 && x < width && y < height) {
        this->grid[y][x] = 1;
        this->grid[y][(x+1) % width] = 1;
        this->grid[(y+1) % height][x] = 1;

        this->grid[(y+2) % height][(x+3) % width] = 1;
        this->grid[(y+3) % height][(x+2) % width] = 1;
        this->grid[(y+3) % height][(x+3) % width] = 1;
    } else {
        std::cerr << "ungültige Koordinaten um Beacon einzufügen" << std::endl;
    }
}

void World::add_methuselah(int x, int y) {
    // Add methuselah pattern, if coordinates are in bounds
    if (x >= 0 && y >= 0 && x < width && y < height) {
      this->grid[y][(x+1) % width] = 1;
      this->grid[y][(x+2) % width] = 1;
      this->grid[(y+1) % height][x] = 1;
      this->grid[(y+1) % height][(x+1) % width] = 1;
      this->grid[(y+2) % height][(x+1) % width] = 1;
    }
    // Print error message if coordinates are out of bounds
    else {
        std::cerr << "ungültige Koordinaten um Methuselah einzufügen" << std::endl;
    }
}

void World::add_random_patterns(int n) {
    std::srand(static_cast<unsigned int>(std::time(0))); // Zufall initialisieren
    for (int i = 0; i < n; ++i) {
        int muster = std::rand() % 4;            // Zufälliges Muster auswählen
        int x = std::rand() % width;             // Zufällige x-Position
        int y = std::rand() % height;            // Zufällige y-Position 
        print();
        if (muster == 0) {
            add_glider(x, y);
            std::cout << "Glider bei (" << x << ", " << y << ") eingefügt.\n";
        } else if (muster == 1) {
            add_toad(x, y);
            std::cout << "Toad bei (" << x << ", " << y << ") eingefügt.\n";
        } else if (muster == 2) {
            add_beacon(x, y);
            std::cout << "Beacon bei (" << x << ", " << y << ") eingefügt.\n";
        } else if (muster == 3) {
            add_methuselah(x, y);
            std::cout << "Methuselah bei (" << x << ", " << y << ") eingefügt.\n";
        }
        std::cout << "\033[2J\033[H";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    }
}
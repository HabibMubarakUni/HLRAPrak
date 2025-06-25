#ifndef WORLD_H           // Verhindert mehrfaches Einbinden der Header-Datei
#define WORLD_H

#include <vector>         // Für die Verwendung von std::vector
#include <string>         // Für die Verwendung von std::string

// Die Klasse World repräsentiert die Welt des Game of Life
class World {
private:
    int height;                                // Höhe der Welt (Anzahl Zeilen)
    int width;                                 // Breite der Welt (Anzahl Spalten)
    int generation;                            // Aktuelle Generation des Spiels
    std::vector<std::vector<int>> grid;        // 2D-Grid zur Speicherung der Weltzustände (0 = tot, 1 = lebendig)

public:
    // Konstruktor: Erstellt eine leere Welt mit gegebener Höhe und Breite
    World(int height, int width);

    // Konstruktor: Lädt eine Welt aus einer Datei (Dateiname wird übergeben)
    World(std::string file_name);

    // Zählt die Anzahl lebender Nachbarn der Zelle an Position (x, y)
    int count_alive_neighbours(int x, int y);

    // Entwickelt die Welt um eine Generation weiter nach den Regeln von Conway's Game of Life
    void evolve();

    // Speichert die aktuelle Welt in eine Textdatei
    void save(std::string file_name);

    // Gibt die aktuelle Welt samt Generation in der Konsole aus
    void print();

    // Prüft, ob zwei Grids identisch sind (z.B. zur Stabilitätsprüfung)
    bool is_equal(const std::vector<std::vector<int>>& grid_1,
                  const std::vector<std::vector<int>>& grid_2);

    // Prüft, ob sich die Welt im stabilen Zustand befindet (d.h. keine Änderung seit letzter Generation)
    bool is_stable();

    // Lädt eine Welt aus einer Datei und initialisiert die Welt damit
    bool load(std::string file_name);

    // Gibt den Zustand der Zelle an Position (x, y) zurück (0 = tot, 1 = lebendig)
    int get_cell_state(int x, int y);

    // Gibt den Zustand der Zelle an einer linearen Position p zurück
    int get_cell_state(int p);

    // Setzt den Zustand der Zelle an Position (x, y) (0 = tot, 1 = lebendig)
    bool set_cell_state(int state, int x, int y);

    // Setzt den Zustand der Zelle an einer linearen Position p
    bool set_cell_state(int state, int p);

    // Fügt ein Glider-Muster an Position (x, y) ein
    void add_glider(int x, int y);

    // Fügt ein Toad-Muster an Position (x, y) ein
    void add_toad(int x, int y);

    // Fügt ein Beacon-Muster an Position (x, y) ein
    void add_beacon(int x, int y);

    // Fügt ein Methuselah-Muster an Position (x, y) ein
    void add_methuselah(int x, int y);

    // Fügt n zufällige kleine Muster an zufälligen Positionen in der Welt ein
    void add_random_patterns(int n);
};

#endif // WORLD_H

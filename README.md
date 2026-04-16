# Blueprint Auto Layout

program służacy do automatycznego układania grafu w przestrzeni 2D na podstawie danych zawartych w pliku.

### wymagania:
cmake,
conan

## budowa:
```
conan install . --output-folder=out/build/conan --build=missing -s build_type=Release
cmake build
```
program został stworzony na platformie windows, inne platformy nie były testowane

## Urzytkowanie:
po wskazaniu pliku z danymi do wczyania program automatycznie wczytuje dane ignorując pozycje wierzchołków zawarte w pliku, nastęmie uruchamia algorytm automatycznego rozmiestrzenia wierzchołków. Po stworzeniu układu program wyświetla graf na ekranie gdzie urzytkownik ma mozliwośc dokładnej ispekcji rezultatu.

do zapisania rezultatu ręcznie wskazać plik gdzie dane grafu zostanę zapisane.

poruszanie kamerą:
- W - góra
- S - dół
- D - prawo
- A - lewo
- Mouse Scroll - zoom

## układ danych:
dane są zapisane w plku w formacie .yaml
```yaml
Nodes:
  - ID: 1
    Name: Event BeginPlay
    X: 0
    Y: 0
  - ID: 2
    Name: Branch
    X: 0
    Y: 0
  - ID: 3
    Name: Play Sound (True)
    X: 0
    Y: 0
  - ID: 4
    Name: Spawn Actor (False)
    X: 0
    Y: 0
Egdes:
  - From: 1
    To: 2
  - From: 2
    To: 3
  - From: 2
    To: 4
```


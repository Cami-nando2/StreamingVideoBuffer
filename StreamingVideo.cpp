#include <iostream>   
#include <deque> // Libreria para manejar el buffer como una cola, ya que sirve para manejar datos por los extremos de una "fila"
#include <cstdlib>    // Libreria para generar numeros aleatorios
#include <ctime>      // Libreria para que los numeros aleatorios cambien cada vez que ejecutas.
#include <thread>     // Para hacer pausas en el programa simulando el tiempo real
#include <chrono>     // Para manejar esas pausas en milisegundos

using namespace std;

int main() {
    srand(time(nullptr)); // Para que los cortes de conexion sean aleatorios cada vez

    //CONFIGURACION
    const int WINDOW_SIZE = 5; // Maximo de datos que el buffer puede guardar (ventana deslizante)
    const int MIN_BUFFER = 3; // Minimo de chunks necesarios para reproducir sin pausas
    const int TOTAL_CHUNKS = 30; // Cuantos chunks se enviaran
    const int FPS_DELAY = 200; // Tiempo entre frames (200 ms = aprox 5/s)

    //VARIABLES
    deque<int> buffer; // Cola donde se almacenan los chunks listos para reproducir
    bool online = true; // Estado de la conexion
    bool reproduciendo = false; // Indica si el video se esta reproduciendo o esta en pausa
    int nextChunk = 1; // Numero del siguiente chunk que llegara
    int reproducidos = 0; // Cuantos chunks se han reproducido
    int tiempoDesconexion = 0; // Tiempo restante de la desconexion simulada

    cout << "-------------------------------------------\n";
    cout << "Simulacion de streaming con Sliding Window\n";
    cout << "-------------------------------------------\n\n";

    // CICLO PRINCIPAL
    // Corre mientras sigan llegando o reproduciendose datos
    while (nextChunk <= TOTAL_CHUNKS || !buffer.empty() || reproduciendo) {

        // 1. Simucion de perdida de conexion aleatoria
        if (online && rand() % 20 == 0) {
            online = false;
            tiempoDesconexion = 3; // Se corta por 3 ciclos
            buffer.clear();        // Al perder conexion, el buffer se vacia
            reproduciendo = false;
            cout << "[RED] Conexion perdida. El video se congela.\n";
        }

        // 2. Manejar reconexion
        if (!online) {
            tiempoDesconexion--;
            if (tiempoDesconexion <= 0) {
                online = true;
                cout << "[RED] Conexion restablecida. Cargando datos...\n";
            } else {
                cout << "[RED] Sin conexion (" << tiempoDesconexion << " ciclos restantes)...\n";
            }
        }

        // 3. Llegada de nuevos datos (solo si hay conexion)
        if (online && buffer.size() < WINDOW_SIZE && nextChunk <= TOTAL_CHUNKS) {
            buffer.push_back(nextChunk); //llega un nuevo trozo del video
            cout << "[RED] Chunk " << nextChunk << " recibido. Buffer: "
                 << buffer.size() << "/" << WINDOW_SIZE << "\n";
            nextChunk++;
        }

        // 4. Comprobar si se puede iniciar o reanudar reproduccion
        if (!reproduciendo) {
            if (buffer.size() >= MIN_BUFFER) {
                reproduciendo = true;
                cout << "[PLAYER] Buffer suficiente (" << buffer.size() 
                     << "). Reanudando reproduccion.\n";
            }
        }

        // 5. Reproduccion (leer y eliminar el primer chunk del buffer)
        if (reproduciendo && !buffer.empty()) {
            int chunk = buffer.front();
            buffer.pop_front();
            reproducidos++;
            cout << "[PLAYER] Reproduciendo chunk " << chunk 
                 << " | Buffer restante: " << buffer.size() << "\n";
        }

        // 6. Pausar si el buffer baja del minimo (para evitar cortes)
        if (reproduciendo && buffer.size() < MIN_BUFFER && nextChunk <= TOTAL_CHUNKS) {
            reproduciendo = false;
            cout << "[PLAYER] Buffer bajo (" << buffer.size() << "/" << MIN_BUFFER 
                 << "). Pausando reproduccion...\n";
        }

        // 7. Salida ordenada y esperas
        cout << "-------------------------------------------\n";
        this_thread::sleep_for(chrono::milliseconds(FPS_DELAY));

        // 8. Detener si ya no quedan datos ni en red ni en buffer
        if (nextChunk > TOTAL_CHUNKS && buffer.empty()) {
            break;
        }
    }

    //FIN DE LA SIMULACION
    cout << "\nStreaming finalizado.\n";
    cout << "Chunks reproducidos: " << reproducidos << " de " << TOTAL_CHUNKS << "\n";
    cout << "Estado final del buffer: " << buffer.size() << "/" << WINDOW_SIZE << "\n";
    cout << "-------------------------------------------\n";

    return 0;
}













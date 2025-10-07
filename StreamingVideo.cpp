#include <iostream>   // Para imprimir en consola (cout)
#include <deque>      // Estructura de datos tipo cola doble (ventana deslizante)
#include <cstdlib>    // Para rand() y srand()
#include <ctime>      // Para inicializar la semilla de rand()
#include <thread>     // Para simular el tiempo real con sleep
#include <chrono>     // Para medir tiempos (milisegundos)

using namespace std;

int main() {
    srand(time(nullptr)); // Semilla para generar numeros aleatorios diferentes cada ejecucion

    //CONFIGURACION DE LA SIMULACION
    const int WINDOW_SIZE = 5; // Capacidad maxima del buffer (ventana deslizante)
    const int MIN_BUFFER = 3; // Minimo de chunks necesarios para reproducir sin pausas
    const int TOTAL_CHUNKS = 30; // Total de datos que se van a simular
    const int FPS_DELAY = 200; // Retraso en milisegundos por frame (5 fps aprox)

    //VARIABLES PRINCIPALES
    deque<int> buffer; // Cola donde se almacenan los chunks listos para reproducir
    bool online = true; // Estado de la conexion
    bool reproduciendo = false; // Indica si el "player" esta reproduciendo
    int nextChunk = 1; // Numero del siguiente chunk que llegara
    int reproducidos = 0; // Cuantos chunks se han reproducido
    int tiempoDesconexion = 0; // Tiempo restante de la desconexion simulada

    cout << "-------------------------------------------\n";
    cout << "Simulacion de streaming con Sliding Window\n";
    cout << "-------------------------------------------\n\n";

    // CICLO PRINCIPAL
    // Continua mientras haya datos por recibir o chunks pendientes en el buffer
    while (nextChunk <= TOTAL_CHUNKS || !buffer.empty() || reproduciendo) {

        // 1. Simular perdida de conexion aleatoria
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
            buffer.push_back(nextChunk);
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












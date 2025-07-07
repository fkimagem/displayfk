#include "TecladoExterno.h"

/**
 * @brief Construct a new TecladoExterno object
 */
TecladoExterno::TecladoExterno() : m_internalTime(0), m_waitForNewDetection(500)
{
    m_tecladoVitual.adPin = 0;
    m_tecladoVitual.keys = NULL;
    m_tecladoVitual.qtdKeys = 0;
    m_tecladoVitual.ultimaLeitura = 0;
    m_tecladoVitual.cacheADValues = new uint16_t[QTD_CACHE];
    fillCachedValues(m_tecladoVitual.cacheADValues, QTD_CACHE, 0);
    /*for(uint8_t i = 0; i < QTD_CACHE; i++){
        tecladoVitual.cacheADValues[i] = 0;
    }*/
}

/**
 * @brief Configura o teclado externo.
 *
 * @param teclado Ponteiro para um array de estruturas keypadExtern_t, cada uma representando um teclado.
 * @param qtdTeclados Quantidade de teclados no array.
 * @param minimunADToConsider Valor minimo lido do ADC para considerar como sinal (quadrado do valor, pois e o valor absoluto do sinal).
 * @param thresholdDeviation Desvio padr o entre os valores lidos para considerar um sinal estavel.
 */
void TecladoExterno::configure(keypadExtern_t *teclado, uint8_t qtdTeclados, uint16_t minimunADToConsider, uint16_t thresholdDeviation)
{
    this->MIN_AD_TO_CONSIDER = minimunADToConsider;
    this->THRESHOLD_DEVIATION = thresholdDeviation;
    this->LIMIAR = (static_cast<uint32_t>(minimunADToConsider) * static_cast<uint32_t>(minimunADToConsider)) / m_divisor;
    this->m_configured = teclado != nullptr && qtdTeclados > 0;
    this->m_teclado = teclado;
    this->m_qtdTeclados = qtdTeclados;
    if (qtdTeclados > 0)
    {
#ifdef LOG_KEYBOARD
        log_d("Configuring ADC Resolution");
#endif

        for (uint8_t i = 0; i < qtdTeclados; i++)
        {
#ifdef LOG_KEYBOARD
            log_d("Configuring pin %d as input", teclado[i].adPin);
#endif
            pinMode(teclado[i].adPin, INPUT);
            teclado[i].cacheADValues = new uint16_t[QTD_CACHE];
        }
    }
}

/**
 * @brief Reads a key from the external keypad
 *
 * @param pressedKey Key that was pressed, if any
 * @return true if a key was pressed, false otherwise
 *
 * Reads the keys from the external keypad and returns true if a key was pressed, and false otherwise.
 * If a key was pressed, the pressedKey parameter is populated with the key that was pressed.
 * The function checks if any key is pressed and then for each key, it checks if the value is stable.
 * If the value is stable and the value is greater than the LIMIAR, it checks if the key is in the list of keys.
 * If the key is in the list, it returns true and populates the pressedKey parameter.
 * If the function does not find any key, it returns false.
 *
 * The function also logs the time it took to read the key, if a key was pressed.
 */
bool TecladoExterno::readKey(keyExtern_t *pressedKey)
{
    if (!m_configured)
    {
#ifdef LOG_KEYBOARD
        log_e("Not configured");
#endif
        return false;
    }
    uint32_t tempo = millis();
    uint8_t indiceTecladoApertado = 0;

    bool algumaTeclaApertada = checkSomeKeyPressed(&indiceTecladoApertado);
    bool encontrouTecla = false;
    if (algumaTeclaApertada)
    {
        fillCachedValues(m_teclado[indiceTecladoApertado].cacheADValues, QTD_CACHE, (analogRead(m_teclado[indiceTecladoApertado].adPin) / m_divisor));
        // for(uint8_t j = 0; j < qtdTeclados; j++){
        //     fillCachedValues(teclado[j].cacheADValues, QTD_CACHE, (analogRead(teclado[j].adPin) / divisor));
        // }
    }

    while (algumaTeclaApertada && m_internalTime + m_waitForNewDetection < millis())
    {
#if !defined(MODO_READONLY) && defined(LOG_KEYBOARD)
        log_d("Entrei loop algumaTeclaApertada %i", indiceTecladoApertado);
#endif
        // Crio o array com leituras atuais e coloco valor 0;
        uint32_t leituraAtual = 0;

        // Para cada teclado, leio e somo as leituras
        for (uint8_t i = 0; i < QTD_LEITURAS; i++)
        {
            leituraAtual += (analogRead(m_teclado[indiceTecladoApertado].adPin) / m_divisor);
        }

        // Divido pelo total de leituras para obter a media
        leituraAtual = leituraAtual / QTD_LEITURAS;

        m_teclado[indiceTecladoApertado].ultimaLeitura = (leituraAtual + m_teclado[indiceTecladoApertado].ultimaLeitura) / 2;
        shiftArray(m_teclado[indiceTecladoApertado].cacheADValues, m_teclado[indiceTecladoApertado].ultimaLeitura, QTD_CACHE);

        float media = 0.0;
        float desvio = 0.0;
        bool estavel = checkDeviation(m_teclado[indiceTecladoApertado].cacheADValues, QTD_CACHE, THRESHOLD_DEVIATION, &media, &desvio); // 2 significa que o devio entre os valores do array nao podem ser maiores que esse valor
        int powA = media;
        powA *= powA;

        // Serial.printf("%d\n", powA);

        if (media >= MIN_AD_TO_CONSIDER && estavel)
        {
#if defined(MODO_READONLY)
            Serial.printf("%d\n", (int)media);
#elif defined(LOG_KEYBOARD)
            log_d("leitura estavel com media: %d", (int)media);
#endif
            encontrouTecla = procurarKey(media, m_teclado[indiceTecladoApertado].keys, m_teclado[indiceTecladoApertado].qtdKeys, pressedKey);
        }
        else if (!estavel)
        {
#if defined(MODO_READONLY)
            log_d("Desvio muito grande: %d", (int)desvio);
#elif defined(LOG_KEYBOARD)
            log_d("Desvio muito grande: %d", (int)desvio);
#endif
        }
        else if (media < MIN_AD_TO_CONSIDER)
        {
#if defined(MODO_READONLY)
            log_d("AD lido muito baixo: %d de %i", (int)media, MIN_AD_TO_CONSIDER);
#elif defined(LOG_KEYBOARD)
            log_d("AD lido muito baixo: %d de %i", (int)media, MIN_AD_TO_CONSIDER);
#endif
        }

        if (encontrouTecla)
        {
            m_internalTime = millis();
        }
        algumaTeclaApertada = checkSomeKeyPressed(&indiceTecladoApertado);
    }

    tempo = millis() - tempo;

    // log_d("Acabou o loop algumaTeclaApertada");

    if (encontrouTecla)
    {
#if !defined(MODO_READONLY) && defined(LOG_KEYBOARD)
        log_d("Elapsed time readKey: %d", tempo);
#endif
        // TODO: verificar a necessidade de resetar (preencher com 0) o cacheADValues
    }

    return encontrouTecla;
}

/**
 * @brief Shifts elements in an array to the left and inserts a new value at the end.
 *
 * @param array Pointer to the array of uint16_t elements to be shifted.
 * @param newValue The new value to be inserted at the end of the array.
 * @param size The size of the array.
 *
 * This function shifts all elements in the given array one position to the left,
 * removing the first element, and inserts the new value at the last position.
 */
void TecladoExterno::shiftArray(uint16_t *array, uint16_t newValue, size_t size)
{
    memmove(array, array + 1, (size - 1) * sizeof(uint16_t));
    array[size - 1] = newValue;
}

/**
 * @brief Looks for a key with a given value in an array of keys.
 *
 * This function takes an array of keys and the value to search for, and looks for
 * the key in the array. It first checks if the value is between the first and last
 * key values, and if so, it returns the respective key. If not, it iterates through
 * the array and returns the key whose average value is closest to the given value.
 *
 * @param valor The value to search for.
 * @param keys The array of keys.
 * @param size The size of the array.
 * @param pressedKey A pointer to a key object to store the found key.
 *
 * @return bool True if a key was found, false otherwise.
 */
bool TecladoExterno::procurarKey(uint32_t valor, keyExtern_t *keys, size_t size, keyExtern_t *pressedKey)
{
    if (size < 2)
    {
        log_e("Size of array is too small");
        return false;
    }
#if !defined(MODO_READONLY) && defined(LOG_KEYBOARD)
    log_d("Looking for key with value: %d", valor);
#endif

    uint32_t primeiraMedia = (keys[0].avgValue + keys[1].avgValue) / 2;             // Como o size minimo é 2, tira a media entre os dois primeiros valores
    uint32_t ultimaMedia = (keys[size - 2].avgValue + keys[size - 1].avgValue) / 2; // Como o size minimo é 2, tira a media entre os dois primeiros valores

    if (valor >= primeiraMedia)
    {
#if !defined(MODO_READONLY)
// log_d("Found first key");
#endif
        *pressedKey = keys[0];
        return true;
    }
    else if (valor <= ultimaMedia)
    {
#if !defined(MODO_READONLY)
// log_d("Found last key");
#endif
        *pressedKey = keys[size - 1];
        return true;
    }
    else
    {
#if !defined(MODO_READONLY)
// log_d("Looking in array");
#endif
        for (int i = 1; i < size; i++)
        {
            uint32_t media = round((keys[i].avgValue + keys[i - 1].avgValue) / 2.0);
            if (valor > media)
            {
                log_d("Key found should be %c(%d) OR %c(%d), avg: %d", keys[i].key, keys[i].avgValue, keys[i - 1].key, keys[i - 1].avgValue, media);
                *pressedKey = keys[i - 1];
                return true;
            }
        }
    }

    return false;
}

/**
 * @brief Checks if the standard deviation of the array is below a given threshold.
 *
 * This function calculates the mean and standard deviation of the provided array.
 * It returns true if the standard deviation is less than or equal to the specified
 * threshold, and false otherwise.
 *
 * @param array Pointer to the array of values.
 * @param size The number of elements in the array.
 * @param threshold The threshold for the standard deviation.
 * @param media Pointer to store the calculated mean of the array.
 * @param desvio Pointer to store the calculated standard deviation.
 * @return true if the standard deviation is less than or equal to the threshold,
 *         false otherwise.
 */
bool TecladoExterno::checkDeviation(uint16_t *array, size_t size, uint16_t threshold, float *media, float *desvio)
{
    float soma = 0.0;
    float somaQuadrados = 0.0;

    // Calcula a soma dos elementos
    for (size_t i = 0; i < size; i++)
    {
        soma += array[i];
    }

    // Calcula a média
    (*media) = soma / size;

    // Calcula a soma dos quadrados das diferenças em relação à média
    for (size_t i = 0; i < size; i++)
    {
        somaQuadrados += ((array[i] - (*media)) * (array[i] - (*media)));
    }

    // Calcula o desvio padrão
    float desvioPadrao = sqrt(somaQuadrados / size);
    (*desvio) = desvioPadrao;

    // Retorna true se o desvio padrão for menor ou igual ao threshold
    return desvioPadrao <= threshold;
}

/**
 * @brief Checks if any key is pressed.
 *
 * This function checks if any of the keys are pressed by reading the analog values of the
 * keys. If the value of any key is greater than the MIN_AD_TO_CONSIDER constant, it returns
 * true, indicating that a key is pressed. Otherwise, it returns false.
 *
 * @return true if any key is pressed, false otherwise.
 */
int TecladoExterno::checkSomeKeyPressed(uint8_t *keyPadIndex)
{
    for (uint8_t j = 0; j < m_qtdTeclados; j++)
    {
        if (analogRead(m_teclado[j].adPin) > MIN_AD_TO_CONSIDER)
        {
            *keyPadIndex = j;
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Fills the cache array with a given value
 * 
 * @param cacheArray Pointer to the cache array
 * @param size Size of the cache array
 * @param valor Value to fill the cache array with
 */
void TecladoExterno::fillCachedValues(uint16_t *cacheArray, uint8_t size, uint16_t valor)
{
    if (size == 0)
        return;

    if (valor == 0 || valor == 0xFFFF)
    {
        memset(cacheArray, (valor == 0 ? 0 : 0xFF), size * sizeof(uint16_t));
    }
    else
    {
        for (uint8_t i = 0; i < size; i++)
        {
            cacheArray[i] = valor;
        }
    }
}

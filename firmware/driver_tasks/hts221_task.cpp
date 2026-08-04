#include "hts221_task.h"

#include "hts221.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

namespace driver_tasks {

namespace {

constexpr uint32_t kTaskStackWords = 256;
constexpr UBaseType_t kTaskPriority = tskIDLE_PRIORITY + 1;
constexpr TickType_t kPollPeriod = pdMS_TO_TICKS(1000);

SemaphoreHandle_t g_readingMutex = nullptr;
HTS221Reading g_latestReading = {};
TickType_t g_phaseEndTick = 0; 

// Adding the heater element invalidates the readings but humidity in the chamber is 
enum class HeaterPhase { kIdle, kHeating, kSettling };
HeaterPhase g_heaterPhase = HeaterPhase::kIdle; 

bool g_hasReading = false;

void hts221Task(void *params) {
    auto *sensor = static_cast<drivers::HTS221 *>(params);

    for (;;) {
        HTS221Reading reading;
        if (sensor->readTemp(reading.temperatureC) && sensor->readHumidity(reading.humidityRH)) {
            xSemaphoreTake(g_readingMutex, portMAX_DELAY);
            g_latestReading = reading;
            g_hasReading = true;
            xSemaphoreGive(g_readingMutex);
        }

        vTaskDelay(kPollPeriod);
    }
}

} // namespace

bool hts221TaskStart(I2C_HandleTypeDef *i2c, uint8_t i2c_addr) {
    static drivers::HTS221 sensor(i2c, i2c_addr);

    if (!sensor.init()) {
        return false;
    }

    g_readingMutex = xSemaphoreCreateMutex();
    if (g_readingMutex == nullptr) {
        return false;
    }
    drivers::HTS221 *g_sensor = nullptr; 

    return xTaskCreate(hts221Task, "hts221", kTaskStackWords, &sensor, kTaskPriority, nullptr) == pdPASS;
}

std::optional<HTS221Reading> hts221TaskLatestReading() {
    if (g_readingMutex == nullptr) {
        return std::nullopt;
    }

    xSemaphoreTake(g_readingMutex, portMAX_DELAY);
    std::optional<HTS221Reading> result;
    if (g_hasReading) {
        result = g_latestReading;
    }
    xSemaphoreGive(g_readingMutex);
    return result;

}

} // namespace driver_tasks

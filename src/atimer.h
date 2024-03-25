/*
    Advanced software timer (based on millis()/micros())
    GitHub: https://github.com/wi1k1n/ATimer
    // Возможности:
    // - Режим таймера и периодичного выполнения
    // - Подключение функции-обработчика
    // - Сброс/запуск/перезапуск/остановка/пауза/продолжение отсчёта
    // - Возможность форсировать переполнение таймера
    // - Возврат оставшегося времени в мс, а также условных единицах 8 и 16 бит
    // - Несколько функций получения текущего статуса таймера
    // - Алгоритм держит стабильный период и не боится переполнения millis()
    
    Ilia Mazlov <mazlov.i.a@gmail.com>, AlexGyver <alex@alexgyver.ru>
    MIT License
*/

#ifndef ATIMER_H__
#define ATIMER_H__

#include <Arduino.h>

enum class ATimerMode {
	ONCE = 0,
	REPEAT
};
enum class ATimerUnit {
	MS = 0,
	US
};

class ATimer {
public:
	ATimer(uint32_t time = 1000, bool isRunning = 0, ATimerMode mode = ATimerMode::ONCE);
	
	// // включить микросекундный режим (true)
	// void setMicros(bool mode) {
	// 	_us = mode;
	// }

	ATimerMode getMode() const;
	ATimerUnit getUnit() const;

	bool isRunning() const;
	
	/// @brief Checks if timer has already fiers. Consumes timer state!
	bool checkIsReady();
	
	void setMode(ATimerMode mode);
	void setUnit(ATimerUnit unit);
	void setTime(uint32_t time);
	
	void start();
	void restart();
	void stop();
	void resume();

	bool tick();

	// // подключить коллбэк
	// void attach(void (*handler)()) {
	// 	_handler = *handler;
	// }
	
	// // отключить коллбэк
	// void detach() {
	// 	_handler = nullptr;
	// }
	
	// // принудительно переполнить таймер
	// void force() {
	// 	_tmr = uptime() - _prd;
	// }

	
	// всегда возвращает true при срабатывании
	bool elapsed() {
		return (uptime() - _tmr >= _prd);
	}
	
	
	// elapsed+active: работает ли таймер + не сработал ли он  
	bool status() {
		return _state && !elapsed();
	}
	
	// остаток времени
	uint32_t timeLeft() {
		return max(long(_prd - _buf), 0L);
	}
	
	// остаток времени в 0-255
	uint8_t timeLeft8() {
		return max(255 - _buf * 255l / _prd, 0ul);
	}
	
	// остаток времени в 0-65535
	uint16_t timeLeft16() {
		return max(65535 - _buf * 65535l / _prd, 0ul);
	}

private:
	/// @brief Time since MCU started (accounts for the units: can be ms or us)
	uint32_t getUptime() const;

private:
	// uint32_t _tmr = 0, _prd = 1000, _buf = 0;
	// bool _state = 0, _mode = 0, _ready = 0, _us = 0;
	// void (*_handler)() = nullptr;
	
	uint32_t _period = 1000; // amount of time to wait between triggers

	uint32_t _timer = 0; // 
	uint32_t _buffer = 0; // keeps track of when the timer was stopped

	bool _isRunning = false;
	bool _isReady = false;

	ATimerMode _mode = ATimerMode::ONCE;
	ATimerUnit _unit = ATimerUnit::MS;
};

inline ATimer::TimerMS2(uint32_t time, bool isRunning, ATimerMode mode) {
	setTime(time);
	if (isRunning)
		start();
	_mode = mode;
}

inline ATimerMode ATimer::getMode() const { return _mode; }
inline ATimerUnit ATimer::getUnit() const { return _unit; }
inline uint32_t ATimer::getUptime() const { return _unit == ATimerUnit::US ? micros() : millis(); }

inline bool ATimer::isRunning() const { return _isRunning; }

inline bool ATimer::checkIsReady() { return _isReady ? (_isReady = 0, 1) : 0; }

inline void ATimer::setMode(ATimerMode mode) { _mode = mode; }
inline void ATimer::setUnit(ATimerUnit unit) { _unit = unit; }
inline void ATimer::setTime(uint32_t time) { _period = (time == 0) ? 1 : time; }

inline void ATimer::start() {
	_isRunning = true;  
	_timer = getUptime();
	_buffer = 0;
}
inline void ATimer::restart() { start(); }
inline void ATimer::stop() {
	if (_isRunning)
		_buffer = getUptime() - _timer;
	_isRunning = false;
};
inline void ATimer::resume() {
	if (!_isRunning)
		_timer = getUptime() - _buffer;
	_isRunning = true;
}

inline bool ATimer::tick() {
	if (_isRunning)
		_buffer = getUptime() - _timer;
	
	if (_isRunning && _buffer >= _period) {
		if (_mode == ATimerMode::REPEAT)
			_timer += _period * (_buffer / _period);
		else
			stop();
		// if (*_handler) _handler();
		_isReady = 1;
		return true;
	}
	return false;
}

#endif // ATIMER_H__
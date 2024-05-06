#include <avr/io.h>
#include <avr/interrupt.h>

// Timer/Counter0 초기화 함수
void timer0_init() {
    TCCR0 |= (1 << CS02) | (0 << CS01) | (1 << CS00); // 분주비 설정 (1024 분주비)
    TIMSK |= (1 << TOIE0); // 오버플로 인터럽트 활성화
    sei(); // 전역적으로 인터럽트 활성화
}

// Timer/Counter0 오버플로우 ISR (1ms마다 호출됨)
ISR(TIMER0_OVF_vect) {
    // do nothing (인터럽트에서 아무 작업도 수행하지 않음)
}

// 밀리초(ms) 단위 딜레이 함수
void delay_ms(uint16_t ms) {
    while (ms) {
        // 1ms마다 인터럽트가 발생하고, 인터럽트 핸들러에서 ms를 감소시킴
        TCNT0 = 0xFF - (uint8_t)((F_CPU / 1024) * 0.001); // 타이머 초기화 (1ms 기준)
        while ((TIFR & (1 << TOV0)) == 0); // 오버플로우 플래그 기다림
        TIFR |= (1 << TOV0); // 오버플로우 플래그 클리어
        ms--;
    }
}

int main(void) {
    DDRA = 0xFF; // 모든 A포트 핀을 출력 모드로 설정

    timer0_init(); // Timer/Counter0 초기화

    while (1) {
        // 채우는 동작
        for (int i = 0; i < 8; i++) {
            PORTA = (0xFF << i); // i번째 비트만 0으로 설정하여 LED 켜기
            delay_ms(1000); // 1초 딜레이
        }

        // 빼는 동작
        for (int i = 8; i >= 0; i--) {
            PORTA = (0xFF << i); // i번째 비트만 0으로 설정하여 LED 켜기
            delay_ms(1000); // 1초 딜레이
        }
    }

    return 0;
}

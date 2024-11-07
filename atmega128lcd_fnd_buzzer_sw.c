#include <avr/io.h>
#include <avr/interrupt.h>

#define CLR 0
#define SET 1
#define TO_START 184

int hundredths = 0;            // 0.01초 단위로 저장하는 변수 (초기값 0)
int tmr0_cnt = 0;              // 타이머 0 카운트 변수
int led_blink_cnt = 0;         // LED 깜빡임을 위한 카운트 변수 (0.1초마다 깜빡임)
char tmr0_flag = CLR;          // 타이머 0 플래그 변수
char timer_running = 0;        // 타이머 동작 여부 (0: 정지, 1: 동작)
char prev_PE4_state = 1;       // 이전 PE4 핀 상태 저장 (1: HIGH, 0: LOW)
char prev_PE5_state = 1;       // 이전 PE5 핀 상태 저장 (1: HIGH, 0: LOW)
volatile int digit_cnt = 0;    // FND 자리 선택 카운트

// 부저 제어 관련 변수
volatile int buzzer_duration = 0; // 부저가 울리는 시간 카운트

// FND 관련 배열 정의
char digit_table[4] = {0x10, 0x20, 0x40, 0x80};  // DIG1~DIG4 자리 제어
char fnd_table[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; // 0~9까지 세그먼트 패턴

// TIMER0 오버플로우 인터럽트 루틴
ISR(TIMER0_OVF_vect)
{
	TCNT0 = TO_START;          // 타이머 초기값 설정

	if (++tmr0_cnt >= 2) {     // 0.01초마다 (2번 오버플로우 -> 0.01초)
		tmr0_cnt = 0;          // 타이머 카운트 초기화
		if (timer_running) {   // 타이머가 동작 중일 때만 증가
			tmr0_flag = SET;   // 타이머 플래그 설정

			// LED 깜빡임 주기 (0.1초마다 깜빡임)
			if (++led_blink_cnt >= 10) { // 0.1초마다 (10 * 0.01초 = 0.1초)
				PORTB ^= 0xFF;          // PORTB의 모든 핀을 토글 (PB0~PB7 깜빡임)
				led_blink_cnt = 0;      // LED 깜빡임 카운트 초기화
			}
		}
		
		// 부저 동작 시간 확인
		if (buzzer_duration > 0) {
			buzzer_duration--;
			if (buzzer_duration == 0) {
				PORTG &= ~(1 << PG3); // 부저 끄기
			}
		}
	}

	// FND 디스플레이 업데이트 (멀티플렉싱)
	PORTD &= ~0xF0; // 현재 자리 끄기
	if (digit_cnt == 0) {
		PORTC = ~fnd_table[(hundredths / 1000) % 10];  // 천의 자리
		PORTD |= digit_table[0];
		} else if (digit_cnt == 1) {
		PORTC = ~fnd_table[(hundredths / 100) % 10];   // 백의 자리
		PORTD |= digit_table[1];
		} else if (digit_cnt == 2) {
		PORTC = ~fnd_table[(hundredths / 10) % 10];    // 십의 자리
		PORTD |= digit_table[2];
		} else if (digit_cnt == 3) {
		PORTC = ~fnd_table[hundredths % 10];           // 일의 자리
		PORTD |= digit_table[3];
	}

	if (++digit_cnt >= 4) digit_cnt = 0;  // 4자리 반복
}

//-------- Delay 서브루틴 --------
void Delay_us(char time_us) {
	char i;
	for (i = 0; i < time_us; i++) {
		asm volatile(" PUSH R0");
		asm volatile(" POP  R0");
		asm volatile(" PUSH R0");
		asm volatile(" POP  R0");
		asm volatile(" PUSH R0");
		asm volatile(" POP  R0");
		asm volatile(" PUSH R0");
		asm volatile(" POP  R0");
	}
}

void Delay_ms(unsigned int time_ms) {
	unsigned int i;
	for (i = 0; i < time_ms; i++) {
		Delay_us(250);
		Delay_us(250);
		Delay_us(250);
		Delay_us(250);
	}
}

//-------- LCD 관련 서브루틴 --------
void lcd_write(char c) {
	PORTA = c;
	PORTG |= 0x04;
	Delay_us(1);
	PORTG &= ~0x04;
	Delay_us(250);
}

void cursor_off(void) {
	PORTG &= 0xFE;
	Delay_ms(200);
	lcd_write(0x0C);
	Delay_ms(100);
}

void lcd_clear(void) {
	PORTG &= 0xFE;
	Delay_us(1);
	lcd_write(0x01);
	Delay_ms(4);
}

void lcd_init(void) {
	PORTG &= 0xFE;
	Delay_ms(200);
	lcd_write(0x38);
	lcd_write(0x0F);
	lcd_write(0x01);
	Delay_ms(100);
}

void lcd_gotoxy(unsigned char x, unsigned char y) {
	PORTG &= 0xFE;
	Delay_us(1);
	if (y == 0) lcd_write(0x80 + x);
	else lcd_write(0xC0 + x);
	Delay_ms(1);
}

void lcd_puts(char *s) {
	PORTG |= 0x01;
	Delay_us(1);
	while (*s) {
		lcd_write(*s++);
	}
}

void lcd_putch(char c) {
	PORTG |= 0x01;
	Delay_us(1);
	PORTA = c;
	PORTG |= 0x04;
	Delay_us(1);
	PORTG &= ~0x04;
	Delay_us(250);
}

void display_time() {
	// LCD에 0.01초 단위로 초를 [XX.XX] 형식으로 표시
	lcd_gotoxy(9,1);                 // 표시 위치로 커서 이동
	lcd_putch('[');                  // 괄호 시작
	lcd_putch((hundredths / 1000) % 10 + '0'); // 초의 10의 자리 출력
	lcd_putch((hundredths / 100) % 10 + '0');  // 초의 1의 자리 출력
	lcd_putch('.');                             // 소수점 출력
	lcd_putch((hundredths / 10) % 10 + '0');    // 소수점 첫째 자리 출력
	lcd_putch(hundredths % 10 + '0');           // 소수점 둘째 자리 출력
	lcd_putch(']');                  // 괄호 끝
}

void check_PE4_PE5(void) {
	// PE4 스위치 상태 확인
	if ((PINE & (1 << PE4)) == 0 && prev_PE4_state == 1) { // falling edge 감지
		timer_running = !timer_running; // 타이머 시작/정지 토글
		display_time(); // 타이머 상태가 변경될 때 항상 현재 시간을 업데이트

		// 타이머 정지 시 LED 끄기
		if (!timer_running) {
			PORTB = 0x00; // PORTB의 모든 LED 끔

			// 타이머 정지 시, 소수점 둘째 자리가 00이면 부저 울림
			if (hundredths % 100 == 0) {  // 0.01초 단위이므로, 100으로 나눈 나머지가 0일 때
				PORTG |= (1 << PG3);     // 부저 켜기
				buzzer_duration = 300;   // 3초 동안 부저 유지 (300 * 0.01초 = 3초)
			}
		}
	}
	prev_PE4_state = (PINE & (1 << PE4)) != 0; // 현재 상태 저장

	// PE5 스위치 상태 확인
	if ((PINE & (1 << PE5)) == 0 && prev_PE5_state == 1) { // falling edge 감지
		hundredths = 0;           // 타이머 리셋
		display_time();           // 리셋된 값 [00.00] 표시

		// 부저 울리기 (0.5초 동안)
		PORTG |= (1 << PG3);  // 부저 켜기
		buzzer_duration = 50; // 0.5초 (50 * 0.01초) 동안 유지
	}
	prev_PE5_state = (PINE & (1 << PE5)) != 0; // 현재 상태 저장
}

int main(void) {
	DDRA = 0xFF;
	PORTA = 0xFF;
	DDRG = 0xFF;
	PORTG = 0x00;
	DDRC = 0xFF;  // FND 데이터 핀을 출력으로 설정
	DDRD = 0xF0;  // FND 자리 선택 핀을 출력으로 설정
	DDRB = 0xFF;  // PB0~PB7를 출력으로 설정 (LED 제어)
	DDRE &= ~(1 << PE4) & ~(1 << PE5); // PE4, PE5 핀을 입력으로 설정
	PORTE |= (1 << PE4) | (1 << PE5);  // PE4, PE5에 풀업 저항 설정

	lcd_init();
	cursor_off();

	// 초기 메시지 표시
	lcd_gotoxy(0,0);
	lcd_puts("Time Watch");
	lcd_gotoxy(0,1);
	lcd_puts("Second = [00.00]");

	// 타이머 설정
	TCCR0 = 0x07;    // 타이머0 설정, 분주비 설정
	TIMSK = 0x01;    // 타이머0 오버플로우 인터럽트 활성화
	TCNT0 = TO_START; // 타이머0 초기화
	sei();           // 전역 인터럽트 활성화

	while (1) {
		check_PE4_PE5();          // PE4, PE5 상태 확인 및 처리

		if (tmr0_flag == SET) {   // 타이머 플래그가 설정되었을 때
			tmr0_flag = CLR;      // 타이머 플래그 초기화

			if (timer_running) {  // 타이머가 동작 중일 때만 0.01초 증가
				if (++hundredths >= 6000) // 60.00초 이상일 때
				hundredths = 0;       // 타이머 리셋

				display_time();           // LCD에 시간 갱신 표시
			}
		}
	}
}

#include <avr/io.h>            // AVR 입출력 라이브러리 포함
#include <avr/interrupt.h>     // AVR 인터럽트 라이브러리 포함

#define CLR 0                  // CLR 상수 정의 (0)
#define SET 1                  // SET 상수 정의 (1)
#define TO_START 184           // 타이머 초기값 상수 정의 (184)

int second;                    // 초를 저장하는 변수
int tmr0_cnt;                  // 타이머 0 카운트 변수
char tmr0_flag;                // 타이머 0 플래그 변수

// TIMER0 OVERFLOW 인터럽트 루틴
ISR(TIMER0_OVF_vect)
{
	TCNT0 = TO_START;          // 타이머 초기값 설정

	if (++tmr0_cnt >= 200) {   // 200번 오버플로우 발생 시
		tmr0_cnt = 0;          // 타이머 카운트 초기화
		tmr0_flag = SET;       // 타이머 플래그 설정
	}
}

//-------- Delay 서브루틴 --------
void Delay_us(char time_us)    // 마이크로초 단위 지연 함수
{
	char i;
	for (i = 0; i < time_us; i++) {
		// 어셈블리 명령어를 이용하여 정확한 지연 생성
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

void Delay_ms(unsigned int time_ms) // 밀리초 단위 지연 함수
{
	unsigned int i;
	
	for (i = 0; i < time_ms; i++) {
		// 1ms마다 250us씩 지연 호출
		Delay_us(250);
		Delay_us(250);
		Delay_us(250);
		Delay_us(250);
	}
}



//-------- LCD 관련 서브루틴 --------
void lcd_write(char c)         // LCD에 문자 쓰기 함수
{
	PORTA = c;                 // PORTA를 통해 데이터 전송
	PORTG |= 0x04;             // enable
	Delay_us(1);
	PORTG &= 0xFB;            // disable
	Delay_us(250);
}

void cursor_off(void)          // 커서 끄기 함수
{
	PORTG &= 0xFE;             // 명령 전송
	Delay_ms(200);
	lcd_write(0x0C);           // 디스플레이 켜고, 커서 끔
	Delay_ms(100);
}

void cursor_on(void)           // 커서 켜기 함수
{
	PORTG &= 0xFE;             // 명령 전송
	Delay_ms(200);
	lcd_write(0x0F);           // 디스플레이 켜고, 커서 켜기
	Delay_ms(100);
}

void lcd_clear(void)           // 화면 지우기 함수
{
	PORTG &= 0xFE;             // 명령 전송
	Delay_us(1);
	lcd_write(0x01);           // 화면 지우기 명령
	Delay_ms(4);
}

void lcd_init(void)            // LCD 초기화 함수
{
	PORTG &= 0xFE;             // 명령 전송
	Delay_ms(200);             // 전원 안정화 지연
	lcd_write(0x38);           // 8비트 모드, 2줄, 5x8 폰트 설정
	lcd_write(0x0F);           // 디스플레이 켜기, 커서 켜기
	lcd_write(0x01);           // 화면 초기화
	Delay_ms(100);
}

void lcd_gotoxy(unsigned char x, unsigned char y) // 커서 위치 설정 함수
{
	PORTG &= 0xFE;             // 명령 전송
	Delay_us(1);
	if (y == 0) lcd_write(0x80 + x); // 첫 번째 줄
	else lcd_write(0xC0 + x);        // 두 번째 줄
}

void lcd_puts(char *s)         // 문자열 출력 함수
{
	PORTG |= 0x01;             // 데이터 전송 설정
	Delay_us(1);
	while (*s) {               // 문자열 끝까지
		lcd_write(*s++);       // 각 문자 출력
	}
}

void lcd_putch(char c)         // 문자 출력 함수
{
	PORTG |= 0x01;             // 데이터 전송 설정
	Delay_us(1);
	PORTA = c;     
	            // 문자 데이터 설정
	PORTG |= 0x04;             // enable
	Delay_us(1);
	
	PORTG &= 0xFB;            // disable
	Delay_us(250);
}

int main(void)
{
	DDRA = 0xFF;               // PORTA를 출력 모드로 설정
	PORTA = 0xFF;
	DDRG = 0xFF;               // PORTG를 출력 모드로 설정
	PORTG = 0x00;

	lcd_init();                // LCD 초기화
	cursor_off();              // 커서 끔

	lcd_gotoxy(0,0);           // 첫 번째 줄, 첫 번째 위치로 커서 이동
	lcd_puts("[Digital Watch]"); // "Digital Watch" 문자열 출력
	lcd_gotoxy(0,1);           // 두 번째 줄, 첫 번째 위치로 커서 이동
	lcd_puts("Second = ");     // "Second = " 문자열 출력

	TCCR0 = 0x07;              // 일반 타이머, clk / 1024 분주비 설정
	TIMSK = 0x01;              // Timer0 Overflow Interrupt 설정
	TCNT0 = TO_START;          // Timer0 초기값 설정

	SREG = 0x80;               // 글로벌 인터럽트 활성화

	while (1) {
		if (tmr0_flag == SET) {    // 타이머 플래그가 설정되었을 때
			tmr0_flag = CLR;       // 타이머 플래그 초기화

			if (++second >= 60)    // 초가 60 이상일 때
			second = 0;        // 초를 0으로 초기화

			lcd_gotoxy(9,1);       // 초 표시 위치로 커서 이동
			lcd_putch(second / 10 + '0'); // 초의 10의 자리 출력
			lcd_putch(second % 10 + '0'); // 초의 1의 자리 출력
		}
	}
}

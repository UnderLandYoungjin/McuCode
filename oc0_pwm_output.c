#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// 입출력 핀을 설정하는 함수
void start_setting(void) {
	DDRB = (1 << PB4); // 14번 핀(OC0A) EnableA 핀을 PWM 제어를 위한 출력핀으로 설정
	DDRF = (1 << PF0) | (1 << PF1); // 61번 Input1과 62번 Input2 모터의 방향을 제어하기 위한 출력핀으로 설정
	DDRD &= ~((1 << PD0) | (1 << PD1)); // 홀 센서의 입력을 위한 입력핀으로 설정
	DDRC = 0xff; // LCD 표시를 위한 출력핀으로 설정
	DDRE = 0xff; // LCD 표시를 위한 출력핀으로 설정
}

void pwm_init(void) {
	// Timer/Counter0 설정
	TCCR0 = (1 << WGM00) | (1 << WGM01) | (1 << COM01) | (1 << CS01); // Fast PWM 모드, Non-inverting 모드, 분주비 8
	OCR0 = 0;
}

int main(void) {
	start_setting(); // 핀 설정 함수 호출
	pwm_init(); // PWM 설정 함수 호출

	// 메인 루프
	while (1) {

		for(float i = 0;  i<= 1 ; i+=0.2)
		{
			OCR0 = 255*i; // Duty cycle 100% (High)
			_delay_ms(2000);
		}
		for(float i = 1;  i>= 0 ; i-=0.2)
		{
			OCR0 = 255*i; // Duty cycle 100% (High)
			_delay_ms(2000);
		}

	}
	
	return 0;
}

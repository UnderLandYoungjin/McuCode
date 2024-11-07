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

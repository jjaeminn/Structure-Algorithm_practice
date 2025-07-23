#include <stdio.h>
#include <stdbool.h> // bool 타입 사용을 위해 필요
#include <string.h>  // strcpy_s, memset 등을 위해 필요
#include <time.h>    // 시간 관련 함수를 위해 필요
// #include <windows.h> // Sleep() 함수를 사용하려면 (Windows 전용)
// #include <unistd.h>  // sleep() 함수를 사용하려면 (Linux/Unix 계열)

#include "data.h" // Person 구조체 정의가 이 파일에 있다고 가정

#define Apartment_floor 25 // 아파트 총 층수 정의 (전역 상수)


// --- 전역 변수 ---
FILE* fp = NULL; // 파일 포인터 (전역으로 유지)
int start_floor = 1; // 엘리베이터의 현재 시작 층 (현재 층)
int apartment_data[Apartment_floor]; // 층별 상태를 나타내는 배열 (0:빈, 1:이동중 등)
struct Person p1; // 사용자 정보를 담을 구조체 변수
bool is_position = true; // 엘리베이터가 안정적인 위치에 있는지 (정지 중이면 true, 이동 중이면 false)
bool is_door_open = true; // 엘리베이터 문이 열려 있는지 (열려 있으면 true, 닫혀 있으면 false)
bool is_foor_onclick = false; // 층 버튼 클릭 여부 (눌렸으면 true)
bool close_button = false; // 닫기 버튼 클릭 여부 (눌렸으면 true)


// --- 함수 선언 ---
// 배열 초기화 함수
void apart_intital(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        *(arr + i) = 0; // 모든 요소를 0으로 초기화
    }
    printf("배열 초기화 완료.\n");
}

// 버튼 클릭 시 상태 변경 함수
bool put_button(int position) {
    if (position < 1 || position > Apartment_floor) {
        printf("오류: 유효하지 않은 층 번호입니다. (1~%d)\n", Apartment_floor);
        return false; // 유효하지 않은 층일 경우 false 반환
    }
    is_foor_onclick = true; // 버튼이 클릭되었음을 표시
    printf("%d층 버튼을 누르셨습니다.\n", position);
    return is_foor_onclick; // true 반환
}

// 엘리베이터 이동 함수
// fs: 현재 층, ps: 목표 층, arrr: 층 상태 배열
// 반환값: 도착한 층 번호 (성공), -1 (오류/이동 불가능)
int moving(int fs, int ps, int* arrr) {
    if (fs == ps) {
        printf("현재 %d층에 있습니다. 같은 층으로 이동할 수 없습니다.\n", fs);
        return fs; // 같은 층이므로 현재 층 반환
    }

    if (!is_position) { // 엘리베이터가 이미 이동 중인 경우
        printf("엘리베이터가 현재 이동 중입니다. 잠시 후 다시 시도해주세요.\n");
        return -1; // 이동 불가
    }

    // 이동 시작 상태 설정
    is_position = false;    // 엘리베이터 이동 시작
    is_door_open = false;   // 문 닫힘

    printf("~~~~~~~~~~이동중~~~~~~~~~~\n");

    // 아래로 이동하는 경우 (fs > ps)
    if (fs > ps) {
        for (int i = fs; i >= ps; i--) {
            printf(" : %d층\n", i);
            // *(arrr + i - 1) = 1; // 이동 중인 층 표시 (배열 인덱스 조정) - 필요하다면 사용
            // Sleep(500); // 0.5초 지연 (시뮬레이션 효과) - 필요시 주석 해제 (windows.h 필요)

            if (i == ps) { // 목표 층에 도착
                printf("도착 완료했습니다 : %d층\n", ps);
                is_position = true;     // 엘리베이터 정지 상태
                is_door_open = true;    // 문 열림

                // 도착 층의 배열 상태 초기화 (예: 호출 완료) - 필요하다면 사용
                // *(arrr + ps - 1) = 0; // 배열 인덱스 조정

                return ps; // 도착한 층 번호 반환
            }
        }
    }
    // 위로 이동하는 경우 (fs < ps)
    else { // if (fs < ps)
        for (int i = fs; i <= ps; i++) {
            printf(" : %d층\n", i);
            // *(arrr + i - 1) = 1; // 이동 중인 층 표시 - 필요하다면 사용
            // Sleep(500); // 0.5초 지연 (시뮬레이션 효과) - 필요시 주석 해제

            if (i == ps) { // 목표 층에 도착
                printf("도착 완료했습니다 : %d층\n", ps);
                is_position = true;     // 엘리베이터 정지 상태
                is_door_open = true;    // 문 열림

                // 도착 층의 배열 상태 초기화 - 필요하다면 사용
                // *(arrr + ps - 1) = 0; // 배열 인덱스 조정

                return ps; // 도착한 층 번호 반환
            }
        }
    }
    // 이 부분은 논리적으로 도달하지 않아야 하지만, 함수의 모든 경로에 반환 값이 필요하므로 추가
    return -1; // 예상치 못한 오류 발생 시
}


// 파일에 현재 시간과 Person 데이터를 기록하는 함수
int writePersonDataToFile(FILE* fp, const char* name, int age, int position) {
    time_t rawtime;
    struct tm info;
    char time_buffer[80];

    time(&rawtime);

    // localtime_s 함수 사용 및 오류 확인
    if (localtime_s(&info, &rawtime) != 0) {
        fprintf(stderr, "localtime_s 함수 오류 발생.\n");
        return 1; // 오류 반환
    }

    // 시간 정보를 원하는 형식으로 포맷팅
    strftime(time_buffer, sizeof(time_buffer), "[%Y-%m-%d %H:%M:%S]", &info);

    // 파일에 데이터 기록
    // time_buffer는 %s로, name은 %-20s, age는 %-5d, position은 %d로 출력
    fprintf(fp, "%s %-20s %-5d %d\n", time_buffer, name, age, position);

    return 0; // 성공 반환
}


// --- 메인 함수 ---
int main() {
    printf("이름 입력: ");
    scanf_s("%s", p1.name, sizeof(p1.name)); // scanf_s는 버퍼 크기를 지정해야 안전

    printf("나이 입력: ");
    scanf_s("%d", &p1.age);

    printf("현재 엘리베이터 층: %d층\n", start_floor); // 엘리베이터의 현재 층 안내

    int destination_floor; // 사용자가 입력할 목표 층
    printf("이동할 층 수 입력 (1 ~ %d): ", Apartment_floor);
    scanf_s("%d", &destination_floor);

    // 입력된 층수가 유효한지 검사
    if (destination_floor < 1 || destination_floor > Apartment_floor) {
        printf("오류: 유효하지 않은 층입니다. 1층에서 %d층 사이의 값을 입력하세요.\n", Apartment_floor);
        return 1; // 프로그램 종료
    }

    // 배열 초기화 함수 호출
    apart_intital(apartment_data, Apartment_floor);

    // 버튼 클릭 함수 호출 (유효성 검사는 put_button 안에서 처리됨)
    if (!put_button(destination_floor)) {
        return 1; // 버튼 클릭 오류 시 프로그램 종료
    }

    // 파일 열기 (moving 함수 호출 전에 열어야 합니다)
    if (fopen_s(&fp, "data.txt", "w") != 0) {
        printf("파일 열기에 실패했습니다.\n");
        return 1;
    }
    else {
        printf("파일 열기에 성공했습니다.\n");
    }

    // 엘리베이터 이동 함수 호출 및 도착 층 반환 받기
    int arrived_pos = moving(start_floor, destination_floor, apartment_data);

    // 이동 함수가 성공적으로 도착 층을 반환했는지 확인
    if (arrived_pos != -1) {
        printf("엘리베이터 이동 완료. 도착 층: %d\n", arrived_pos);
        // writePersonDataToFile 함수 호출 시, arrived_pos를 position으로 전달
        if (writePersonDataToFile(fp, p1.name, p1.age, arrived_pos) != 0) {
            printf("데이터 파일 기록 중 오류 발생.\n");
            // 오류 발생 시 파일 닫고 종료
            fclose(fp);
            return 1;
        }
        printf("데이터를 파일에 성공적으로 기록했습니다.\n");
    }
    else {
        printf("엘리베이터 이동이 실패했거나, 현재 이동할 수 없습니다.\n");
    }

    // 파일 닫기
    if (fp != NULL) { // fp가 유효한지 다시 한번 확인
        fclose(fp);
    }

    // 디버그 모드일 때 배열 상태 출력
#ifdef DEBUG // #ifdef _DEBUG 대신 #ifdef DEBUG 사용 (매크로 이름 일치)
    printf("\n--- 최종 배열 상태 확인 (DEBUG 모드) ---\n");
    for (int i = 0; i < Apartment_floor; i++) {
        printf("apartment_data[%d]: %d\n", i, apartment_data[i]);
    }
    printf("--------------------------------------\n");
#endif

    return 0; // 프로그램 정상 종료
}
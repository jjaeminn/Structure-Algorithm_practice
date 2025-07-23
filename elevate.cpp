#include <stdio.h>
#include <stdbool.h> // bool 타입 사용을 위해 필요
#include <string.h>  // strcpy_s 등을 위해 필요
#include <time.h>    // 시간 관련 함수를 위해 필요

#include "data.h" // Person 구조체 정의가 이 파일에 있다고 가정

#define Apartment_floor 25 // 아파트 총 층수 정의 (전역 상수)


// --- 전역 변수 ---
FILE* fp = NULL;
int start_floor = 1; // 엘리베이터의 현재 시작 층 (현재 층)
int apartment_data[Apartment_floor]; // 층별 상태를 나타내는 배열 (0:빈, 1:이동중 등)
struct Person p1; // 사용자 정보를 담을 구조체 변수 (전역으로 유지)
bool is_position = true; // 엘리베이터가 안정적인 위치에 있는지 (정지 중이면 true, 이동 중이면 false)
bool is_door_open = true; // 엘리베이터 문이 열려 있는지 (열려 있으면 true, 닫혀 있으면 false)
bool is_foor_onclick = false; // 층 버튼 클릭 여부 (눌렸으면 true)
bool close_button = false; // 닫기 버튼 클릭 여부 (눌렸으면 true)


// --- 함수 선언 ---
void apart_intital(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        *(arr + i) = 0; 
    }
    printf("배열 초기화 완료.\n");
   
    /*
    printf("초기화된 배열 내용:\n");
    for (int i = 0; i < size; i++) {
        printf("arr[%d]: %d\n", i, arr[i]);
    }
    */
}


bool put_button(int position) {
    if (position < 1 || position > Apartment_floor) {
        printf("오류: 유효하지 않은 층 번호입니다. (1~%d)\n", Apartment_floor);
        return false; 
    }
    is_foor_onclick = true; // 버튼이 클릭되었음을 표시
    printf("%d층 버튼을 누르셨습니다.\n", position);
    return is_foor_onclick; // true 반환
}

// 엘리베이터 이동 함수
// fs: 현재 층, ps: 목표 층, arrr: 층 상태 배열 (이 함수 내에서 직접 조작하지 않으므로 사용법 확인 필요)
// 반환값: 도착한 층 번호 (성공), -1 (오류/이동 불가능)
int moving(int fs, int ps, int* arrr) {
    if (fs == ps) {
        printf("현재 %d층에 있습니다. 같은 층으로 이동할 수 없습니다.\n", fs);
        return fs; 
    }

    if (!is_position) { 
        printf("엘리베이터가 현재 이동 중입니다. 잠시 후 다시 시도해주세요.\n");
        return -1; 
    }

    // 이동 시작 상태 설정
    is_position = false;    
    is_door_open = false;   

    printf("~~~~~~~~~~이동중~~~~~~~~~~\n");

   
    if (fs > ps) {
        for (int i = fs; i >= ps; i--) {
            printf(" : %d층\n", i);
          

            if (i == ps) { // 목표 층에 도착
                printf("도착 완료했습니다 : %d층\n", ps);
                is_position = true;     // 엘리베이터 정지 상태
                is_door_open = true;    // 문 열림
              
                return ps; // 도착한 층 번호 반환
            }
        }
    }
   
    else { 
        for (int i = fs; i <= ps; i++) {
            printf(" : %d층\n", i);
         

            if (i == ps) { // 목표 층에 도착
                printf("도착 완료했습니다 : %d층\n", ps);
                is_position = true;     // 엘리베이터 정지 상태
                is_door_open = true;    // 문 열림
                // *(arrr + ps - 1) = 0; // 도착 층의 배열 상태 초기화 - 필요하다면 주석 해제 후 사용
                return ps; 
            }
        }
    }
    
    return -1; 
}



int writePersonDataToFile(FILE* file_ptr, const char* name, int age, int position) {
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

    // 파일에 데이터 기록 (매개변수로 받은 file_ptr 사용)
    fprintf(file_ptr, "%s %-20s %-5d %d\n", time_buffer, name, age, position);

    return 0; // 성공 반환
}




// --- 메인 함수 ---
int main() {
    // 사용자 정보 입력
    printf("이름 입력: ");
    scanf_s("%s", p1.name, sizeof(p1.name)); // scanf_s는 버퍼 크기를 지정해야 안전

    printf("나이 입력: ");
    scanf_s("%d", &p1.age);

    printf("현재 엘리베이터 층: %d층\n", start_floor); // 엘리베이터의 현재 층 안내

    int destination_floor; // 사용자가 입력할 목표 층

    if (fopen_s(&fp, "data.txt", "a") != 0) { // !!! 여기서 "a" (append) 모드로 변경 !!!
        printf("파일 열기에 실패했습니다.\n");
        return 1;
    }
    else {
        printf("파일 열기에 성공했습니다.\n");
    }

    while (true) { 
        printf("\n이동할 층 수 입력 (1 ~ %d, 종료하려면 0 입력): ", Apartment_floor);
        if (scanf_s("%d", &destination_floor) != 1) {
            printf("잘못된 입력입니다. 숫자를 입력해주세요.\n");
            // 입력 버퍼 비우기 (무한 루프 방지)
            while (getchar() != '\n');
            continue; 
        }

        if (destination_floor == 0) {
            printf("엘리베이터 시뮬레이션을 종료합니다.\n");
            break; // 0을 입력하면 루프 종료
        }

        
        if (destination_floor < 1 || destination_floor > Apartment_floor) {
            printf("오류: 유효하지 않은 층입니다. 1층에서 %d층 사이의 값을 입력하세요.\n", Apartment_floor);
            continue; 
        }

     
        apart_intital(apartment_data, Apartment_floor);

  
        if (!put_button(destination_floor)) {
            continue; 
        }

  
        int arrived_pos = moving(start_floor, destination_floor, apartment_data);

      
        if (arrived_pos != -1) {
            printf("엘리베이터 이동 완료. 도착 층: %d\n", arrived_pos);
            if (writePersonDataToFile(fp, p1.name, p1.age, arrived_pos) != 0) {
                printf("데이터 파일 기록 중 오류 발생.\n");
                fclose(fp);
                return 1;
            }
            printf("데이터를 파일에 성공적으로 기록했습니다.\n");
            start_floor = arrived_pos; 
        }
        else {
            printf("엘리베이터 이동이 실패했거나, 현재 이동할 수 없습니다.\n");
        }
    } 
    if (fp != NULL) {
        fclose(fp);
        printf("\ndata.txt 파일이 성공적으로 닫혔습니다.\n");
    }

    // 디버그 모드일 때 배열 상태 출력
#ifdef DEBUG
    printf("\n--- 최종 배열 상태 확인 (DEBUG 모드) ---\n");
    for (int i = 0; i < Apartment_floor; i++) {
        printf("apartment_data[%d]: %d\n", i, apartment_data[i]);
    }
    printf("--------------------------------------\n");
#endif

    return 0; // 프로그램 정상 종료
}

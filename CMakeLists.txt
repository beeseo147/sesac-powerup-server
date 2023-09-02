# CMakeList.txt : 최상위 CMake 프로젝트 파일, 전역 구성을 수행하고
# 여기에 하위 프로젝트를 포함합니다.

# CMake 버전 및 설정
cmake_minimum_required (VERSION 3.8)
set (CMAKE_SUPPRESS_REGENERATION true)

# 프로젝트 이름
project ("sesac-powerup-server")

# 하위 프로젝트
add_subdirectory ("Common")
add_subdirectory ("Server")
add_subdirectory ("TestClient")

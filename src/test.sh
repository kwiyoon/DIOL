
PROGRAM="./testDIOL"  # 실행 파일 경로를 지정하세요

# "../src/test/dataset/" 디렉터리에 있는 모든 파일명을 배열에 저장합니다.
files=("../src/test/dataset/workload/"*)
size=10000000
# 파일명을 하나씩 인자로 전달하여 프로그램을 실행합니다.
for file in "${files[@]}";
do
  echo "Test_${file}"
  $PROGRAM $size "$file"
done

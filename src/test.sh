
PROGRAM="./testDIOL"  # 실행 파일 경로를 지정하세요

size=100000000

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <value>"
  echo "Allowed values: 0.05, 0.1, 0.2, 0.3"
  exit 1
fi

VALUE=$1

# data와 files 배열 설정
case $VALUE in
  0.05)
    data=("data_c100000000_d0.05")
    files=("workloadA_data_c100000000_d0.05" "workloadB_data_c100000000_d0.05" "workloadC_data_c100000000_d0.05" "workloadD_data_c100000000_d0.05" "workloadE_data_c100000000_d0.05" "workloadF_data_c100000000_d0.05" "workloadG_data_c100000000_d0.05")
    ;;
  0.1)
    data=("data_c100000000_d0.10")
    files=
    ;;
  0.2)
    data=("data_c100000000_d0.20")
    files=
    ;;
  0.3)
    data=("data_c100000000_d0.30")
    files=
    ;;
  *)
    echo "Invalid value: $VALUE"
    echo "Allowed values: 0.05, 0.1, 0.2, 0.3"
    exit 1
    ;;
esac

# 배열 내용 출력
for file in "${files[@]}"; do
    echo "[ IoRDB ] Running with datas=${data} and workload=${file}"
    $EXECUTABLE $size $data $file
done
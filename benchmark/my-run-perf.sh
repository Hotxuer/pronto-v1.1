rm log.txt output.txt log.csv
touch log.txt output.txt 

echo ----------"begin to init ext4----------"
cd ..
./init_ext4.sh
if [[ $? -ne 0 ]]; then
    echo "Unable to init ext4"
    exit 1
fi
echo ----------"begin to remake src----------"
cd src
make clean && make
if [[ $? -ne 0 ]]; then
    echo "Unable to remake src"
    exit 1
fi

echo ----------"begin to remake benchmark----------"
cd ../benchmark
make clean && make
if [[ $? -ne 0 ]]; then
    echo "Unable to remake benchmark"
    exit 1
fi

echo ----------"begin to run benchmark----------"
export USE_SMALL_DATASET=1
./run-perf.sh

echo ----------"begin to parse log----------"
python3 parse_data.py log.txt
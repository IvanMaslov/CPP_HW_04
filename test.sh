for((i = 0; i < 100; i++))
do
./ProjectCPP --mode=debug a b
echo "test $i"
cmp --silent a b || echo "error"
done
sleep 10

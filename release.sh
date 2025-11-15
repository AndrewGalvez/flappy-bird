make rebuild
mv game build/
zip -r $2-linux.zip build/
gh release new $1
gh release upload $1 $2-linux.zip

make wasm
mkdir $2
mv index.* ./$2
zip -r wasm.zip $2/
gh release upload $1 wasm.zip
rm wasm.zip
rm -rf $2
cd ../website-react
nvim public/games.json
./update-builds.sh

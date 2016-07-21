# aarch64向けASP3カーネルプロトタイプ版メモ

## コンパイル手順

クロスコンパイラとして gcc-linaro-5.3-2016.02のlinux向けaarch64クロス
コンパイラ(*)を使用しています。

(*) https://releases.linaro.org/components/toolchain/binaries/latest-5/aarch64-linux-gnu/gcc-linaro-5.3-2016.02-x86_64_aarch64-linux-gnu.tar.xz

aarch64-linux-gnu-gccへのパスを環境変数PATHに設定して, 以下の手順を実行します。

* 本リポジトリをgit cloneコマンドによりローカルディレクトリにクローンす
る
* クローン先に移る(cd asp3-aarch64を実行する)
* mkdir OBJを実行
* cd OBJ を実行
* ../configure.rb -T aarch64_gcc を実行
* make を実行

完了するとカレントディレクトリにasp3というカーネルイメージができます。

## QEmuによる実行
OBJディレクトリで,
```qemu-system-aarch64 -M virt -cpu cortex-a53  -kernel asp```
を実行するとQEmuエミュレータ上でasp3カーネルが動作し, 附属のサンプルプ
ログラムが動作します。

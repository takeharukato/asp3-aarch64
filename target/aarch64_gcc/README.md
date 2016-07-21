# aarch64向けASP3カーネルプロトタイプ版メモ

## コンパイル手順

以下の手順を実行します。

* 本リポジトリをgit cloneコマンドによりローカルディレクトリにクローンす
る
* クローン先に移る(cd asp3-aarch64を実行する)
* mkdir OBJを実行
* cd OBJ を実行
* make を実行

完了するとカレントディレクトリにasp3というカーネルイメージができます。

## QEmuによる実行
OBJディレクトリで,
```qemu-system-aarch64 -M virt -cpu cortex-a53  -kernel asp```
を実行するとQEmuエミュレータ上でasp3カーネルが動作し, 附属のサンプルプ
ログラムが動作します。

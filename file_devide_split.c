#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//linuxコマンドを使ってファイル分割できた
int main(int argc, char **args)
{
    char* size = " 20K";//20Kに分割する 2Mの前にspaceを入れる
    char* filename =" ./graph_img/network_bandwidth.png";// 分割したいファイル 名前の前にspaceを入れる

    char command_split[100] = "split -d -b";
    strcat(command_split, size);
    strcat(command_split, filename);
    strcat(command_split, " devided_file.");//分割後のファイルの名前はdevided_file

    if(system(command_split) == -1){
        printf("コマンド失敗");
    }

    char command_cat[100] = "cat devided_file.* > reframed_file";//分割したファイルを再構成する。

    
    if(system("cat devided_file.* > reframed_file") == -1){
        printf("コマンド失敗");
    }

    return (0);
}


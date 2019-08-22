#include <iostream>
#include <cstring>
#include <winsock2.h>

#include "Email.h"

using namespace std;

Email::Email(char *toAddr) {
    strcpy(recMail, toAddr);
    cout << recMail;
}

SOCKET Email::connectServer(const char *host, int port) {
    WSAData wd;
    SOCKET sock;
    SOCKADDR_IN saddr;
    struct hostent *pHostent;

    if (WSAStartup(MAKEWORD(2, 2), &wd) != 0) {
        return 0;
    }

    if (HIBYTE(wd.wVersion) != 2 || LOBYTE(wd.wVersion) != 2) {
        cout << "初始化失败" << endl;
        WSACleanup();
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    pHostent = gethostbyname(host);

    saddr.sin_addr.S_un.S_addr = *((unsigned long*)pHostent->h_addr_list[0]);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);

    connect(sock, (SOCKADDR*)&saddr, sizeof(SOCKADDR));
    return sock;
}

void Email::recMsg(SOCKET sock) {
    char text[BUFSIZ];
    strset(text, '\0');
    recv(sock, text, BUFSIZ, 0);
    cout << "recv:" << text << endl;
}

void Email::sendMsg(SOCKET sock, char* msg) {
    send(sock, msg, strlen(msg), 0);
}

char* Email::base64_encode(const char* data, int data_len) {
    const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    int prepare = 0; 
    int ret_len; 
    int temp = 0; 
    char *ret = NULL; 
    char *f = NULL; 
    int tmp = 0; 
    char changed[4]; 
    int i = 0; 
    ret_len = data_len / 3; 
    temp = data_len % 3; 
    if (temp > 0) {
        ret_len += 1; 
    } 
    ret_len = ret_len*4 + 1; 
    ret = (char*)malloc(ret_len); 
    if ( ret == NULL) { 
        cout << "内存不足" << endl; 
        exit(0); 
    }
    memset(ret, 0, ret_len); 
    f = ret; 
    while (tmp < data_len) { 
        temp = 0; 
        prepare = 0; 
        memset(changed, '\0', 4); 
        while (temp < 3) { 
            if (tmp >= data_len) break; 
            prepare = ((prepare << 8) | (data[tmp] & 0xFF)); 
            tmp++; 
            temp++; 
        } 
        prepare = (prepare<<((3-temp)*8)); 
        for (i = 0; i < 4 ;i++ ) { 
            if (temp < i) { 
                changed[i] = 0x40; 
            } else { 
                changed[i] = (prepare>>((3-i)*6)) & 0x3F; 
            } 
            *f = base[changed[i]];
            f++; 
        } 
    } 
    *f = '\0'; 
    return ret;
} 

void Email::sendTo() {
    sock = connectServer(host, port);

    //接受服务器发来的数据
    recMsg(sock);

    //跟服务器打招呼
    sendMsg(sock, "helo 163.com\r\n");

    //接受服务器发来的数据
    recMsg(sock);

    //告诉服务器要登陆
    sendMsg(sock, "auth login\r\n");

    //接受服务器发来的数据
    recMsg(sock);

    strcpy(sendMail, "q961017025@163.com");
    strcpy(pass, "asdf1234567");
    // strcpy(recMail, "q961017025@163.com");

    //发送者的邮箱
    pstr = base64_encode(sendMail, strlen(sendMail));
    strcpy(sendBuf, pstr);
    strcat(sendBuf, "\r\n\0");
    sendMsg(sock, sendBuf);

    //接受服务器发来的数据
    recMsg(sock);
    free(pstr);

    //发送者的密码
    pstr = base64_encode(pass, strlen(pass));
    strcpy(sendBuf, pstr);
    strcat(sendBuf, "\r\n\0");
    sendMsg(sock, sendBuf);
    
    //接受服务器发来的数据
    recMsg(sock);
    free(pstr);

    //发件人信息
    strcpy(sendBuf, "mail from:<");
    strcat(sendBuf, sendMail);
    strcat(sendBuf, ">");
    strcat(sendBuf, "\r\n\0");
    sendMsg(sock, sendBuf);

    //接受服务器发来的数据
    recMsg(sock);

    //收件人信息
    strcpy(sendBuf, "rcpt to:<");
    strcat(sendBuf, recMail);
    strcat(sendBuf, ">");
    strcat(sendBuf, "\r\n\0");
    sendMsg(sock, sendBuf);

    //接受服务器发来的数据
    recMsg(sock);

    //邮件内容
    strcpy(subject, "登陆密码");
    strcpy(content, "test");

    sendMsg(sock, "data\r\n\0");
    recMsg(sock);

    strcat(sendBuf, "subject:");
    strcat(sendBuf, subject);
    strcat(sendBuf, "\r\n\r\n");
    strcat(sendBuf, content);
    strcat(sendBuf, "\r\n.\r\n\0");
    sendMsg(sock, sendBuf);
    
    //接受服务器发来的数据
    recMsg(sock);

    //断开连接
    closesocket(sock);
    WSACleanup();
}
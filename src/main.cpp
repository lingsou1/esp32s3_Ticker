/*
接线说明:无

程序说明:本程序实现多任务处理(通过Ticker库来实现)

注意事项:1):这个Ticker不能在其中执行较多的任务;
           如果要实现多任务处理较为复杂的任务,可以使用Ticker计数来实现,
           Ticker只负责计数,当计数到某个值后即在loop()函数中触发所需的任务
        2):请注意：attach函数所能传递的参数最多只有一个。
           另外该参数仅能是以下类型中的一种：
           char, short, int, float, void*, char*。

函数示例:

作者:灵首

时间:2023_4_7

*/

#include <Arduino.h>
#include <Ticker.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>

#define LED_A 10
#define LED_B 11

//需要建立的Ticker任务对象,不同任务用不同的名字
Ticker ticker;
Ticker serPri;

//用来计数
static int count;
static int httpReqCountNum;

//建立WiFiMulti 的对象,对象名称是 wifi_multi
WiFiMulti wifi_multi;  

//需要HTTP访问的地址
#define URL  "http://www.example.com"   //是不能用等号的吗



/*
# brief 连接WiFi的函数
# param 无
# retval 无
*/
void wifi_multi_con(void){
  int i=0;
  while(wifi_multi.run() != WL_CONNECTED){
    delay(1000);
    i++;
    Serial.print(i);
  }

}



/*
# brief 写入自己要连接的WiFi名称及密码,之后会自动连接信号最强的WiFi
# param 无
# retval  无
*/
void wifi_multi_init(void){
  wifi_multi.addAP("haoze1029","12345678");
  wifi_multi.addAP("LINGSOU12","123455678");
  wifi_multi.addAP("LINGSOU1","123455678");
  wifi_multi.addAP("LINGSOU234","12345678");   //通过 wifi_multi.addAP() 添加了多个WiFi的信息,当连接时会在这些WiFi中自动搜索最强信号的WiFi连接
}



/*
# brief   Ticker任务:用来串口输出简单的文本
# param   int num :需传入的参数,用来控制串口输出次数
# retval    无
*/
void hi_world(int num){
  count++;
  if(count <= num){
    Serial.print("hello IOT world : ");
    Serial.print(count);
    Serial.print("\n");
  }
  else{
    serPri.detach();    //结束这个任务
    Serial.print("ticker.detach()");
    Serial.print("\n");
  }
}



/*
# brief   Ticker的任务:计数,当计数到一定值后运行某段程序
# param   无
# retval    无
*/
void httpReqCounter(){
  Serial.print("httpReqCountNum =  ");
  Serial.print(httpReqCountNum);
  httpReqCountNum++;
  Serial.print("\n");
}



/*
# brief  通过HTTP请求获取指定的网址的相关信息
# param  String URL_REQUESTED:对应的需要请求的网址
# retval  无
*/
void httpClientRequest(String URL_REQUESTED){
  //实例化,创建一个HTTPClient对象
  HTTPClient httpClient;    

  //通过 httpClient.begin()配置请求地址,可以只使用网址来设置请求
  httpClient.begin(URL_REQUESTED);
  Serial.print("URL:");
  Serial.println(URL_REQUESTED);
  Serial.print("\n");

  //使用GET()函数启动连接并发送HTTP请求,同时获取状态码'
  //根据状态码来进行下一步的动作
  int httpCode = httpClient.GET();
  Serial.print("send request to url : ");
  Serial.print(URL_REQUESTED);
  Serial.print("\n");

  //根据状态码来进行操作
  //如果是200,则说明请求成功,则要求在串口返回请求payload的字符串
  //(就是从服务器获取响应体信息并通过串口输出)
  if(httpCode == HTTP_CODE_OK){
    String responsePayload = httpClient.getString();
    Serial.print("Serve response payload:\n");
    Serial.print(responsePayload);
  }
  //获取失败则输出错误的状态码
  else{
    Serial.print("fail , serve response code:");
    Serial.print(httpCode);
    Serial.print("\n");
  }
  //关闭HTTP连接
  httpClient.end();
}



void setup() {
  // 连接串口
  Serial.begin(9600);
  Serial.print("serial is OK\n");

  // 设置按键引脚,这是输出模式
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  digitalWrite(LED_A, 0);
  digitalWrite(LED_B, 0);

  //wifi 连接设置
  wifi_multi_init();
  wifi_multi_con();
  Serial.print("wifi connected!!!\n");

  //Ticker 多线程任务
  ticker.attach(2,httpReqCounter);  //应该是10秒执行一次
  serPri.attach(1,hi_world,20); //1秒执行一次打印输出,并向函数中传递参数
}



void loop() {
  if(httpReqCountNum >= 5){
    httpReqCountNum = 0;
    httpClientRequest(URL);

    //当发出HTTP请求时亮灯
    digitalWrite(LED_A,1);
    digitalWrite(LED_B,1);
  }
  else if(httpReqCountNum ){
    //未发出HTTP请求时灭灯
    digitalWrite(LED_A,0);
    digitalWrite(LED_B,0);
  }
}
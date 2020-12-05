#include <Mouse.h>
#include "myLambdas.h"  //使用する関数を別ファイルにまとめました。

constexpr auto MOUSE_BUTTON ( 2 );  //マウスボタンをピン2につないでます。
constexpr auto X_AX ( A0 ); // X軸の出力をピンA0につないでます。
constexpr auto Y_AX ( A1 ); // Y軸の出力をピンA1につないでます。

constexpr auto THRESHOLD ( 0 ); //マウスを動かす最低距離。これ未満は反応しない。
constexpr auto RANGE ( 5 );    //マウスを動かす最大距離
constexpr auto RESPONSE_TIME ( 10); // 動作安定と速度調整のための遅延
constexpr auto CENTER_X ( 526 );    // X軸の中心値。別途計測ししました。
constexpr auto CENTER_Y ( 504 );    // Y軸の中心値。別途計測しました。

auto smoothX ( myLambdas::smoothF() );  // X軸のデータを平滑化する関数
auto smoothY ( myLambdas::smoothF() );  //  Y軸のデータを平滑化する関数

auto debounceButton ( myLambdas::debounceF() ); // ボタンのデータをデバウンスする関数
auto isRaisedButton ( myLambdas::isRaisedF() ); //ボタンのデータが falseー>true に変化したか を返す関数
auto isDroppedButton ( myLambdas::isDroppedF() ); //ボタンのデータが trueー>false に変化したか を返す関数

//ボタンのデータによってボタンを動かす関数
auto buttonActionByData (
  [](const bool DATA_BUTTON) -> void {
    if( isRaisedButton( DATA_BUTTON ) )   Mouse.release( MOUSE_LEFT );
    if( isDroppedButton( DATA_BUTTON ) )  Mouse.press( MOUSE_LEFT );
    return;    
  }
);

//X軸、Y軸のデータとTHRESHOLDよってマウスを動かす関数
auto mouseActionByDataF (
  []( const int THRESHOLD ){
    return [=]( const int DATA_X, const int DATA_Y ) mutable -> void {
      if( abs( DATA_X ) < THRESHOLD && abs( DATA_Y ) < THRESHOLD ) return;
      Mouse.move( DATA_X, -DATA_Y, 0 );
      return;    
    };
  }
);
auto mouseActionByData ( mouseActionByDataF( THRESHOLD ) );

//0 ~ CENTER ~ 1023 のデータを-RANGE ~ 0 ~ RANGE にマッピングする関数
//正負それぞれ3ブロックに分け、傾きを変えることでAカーブを近似する
auto mapToRangeF ( 
  []( const int CENTER, const int RANGE ){
    return [=]( const int DATA ) mutable -> int {
      const auto OFFSET (DATA - CENTER );
      const auto IN_MAX ( 1024 );
      const auto EDGE_M3 ( - CENTER );
      const auto EDGE_M2 (  EDGE_M3 * 3 / 4);
      const auto EDGE_M1 (  EDGE_M3 / 2 );
      const auto EDGE_P3 ( IN_MAX - CENTER );
      const auto EDGE_P2 ( EDGE_P3 * 3 /4 );
      const auto EDGE_P1 ( EDGE_P3 / 2 ); 
      if( OFFSET < EDGE_M2 )  return map( OFFSET, EDGE_M2, EDGE_M3, - RANGE / 2, - RANGE     ); 
      if( OFFSET < EDGE_M1 )  return map( OFFSET, EDGE_M1, EDGE_M2, - RANGE / 4, - RANGE / 2 );
      if( OFFSET < 0       )  return map( OFFSET,       0, EDGE_M1,           0, - RANGE / 4 );
      if( OFFSET <= EDGE_P1 ) return map( OFFSET,       0, EDGE_P1,           0,   RANGE / 4 );
      if( OFFSET <= EDGE_P2 ) return map( OFFSET, EDGE_P1, EDGE_P2,   RANGE / 4,   RANGE / 2 );  
                              return map( OFFSET, EDGE_P2, EDGE_P3,   RANGE / 2,   RANGE     );
    };
  }
);
auto mapToRangeX (mapToRangeF( CENTER_X, RANGE ) );
auto mapToRangeY (mapToRangeF( CENTER_Y, RANGE ) );

void setup() {
  pinMode( MOUSE_BUTTON, INPUT_PULLUP );  
  Mouse.begin();    
}

void loop() {
  //マウスボタンのピンを読んでデバウンスしたデータでボタンを動かす
    buttonActionByData( debounceButton( digitalRead( MOUSE_BUTTON ) ) ); 
  //X軸、Y軸のピンを読んで平滑化して移動距離にマッピングしたデータでマウスを動かす
    mouseActionByData( mapToRangeX( smoothX( analogRead( X_AX ) ) )
                    , mapToRangeY( smoothY( analogRead( Y_AX ) ) ) );
    delay( RESPONSE_TIME );
}

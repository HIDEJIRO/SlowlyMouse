namespace myLambdas
{
  //以前の値と新しい値の加重平均を返すクロージャ
  auto smoothF ( 
    []( const long RATE = 20 ){
      long previousValue ( 0 );
      return [=]( const long CURRENT_VALUE ) mutable -> long {       
        return previousValue = (RATE * CURRENT_VALUE + (100 - RATE) * previousValue) / 100;
      };
    } 
  );

  //以前の値かける100と新しい値かける100の荷重平均を取り、
  //その値と閾値によってtrueまたはfalseを返すクロージャ
  auto debounceF (
    []( const long RATE = 25, const long THRESHOLD_L = 10, const long THRESHOLD_H = 90 ){
      long previousValueMul100 ( 100 );
      bool debouncedValue ( true ); 
      return [=]( const long CURRENT_VALUE ) mutable -> bool {
        previousValueMul100 = RATE * CURRENT_VALUE + (100 - RATE) * previousValueMul100 / 100;
        if( debouncedValue ) {
          if( previousValueMul100 < THRESHOLD_L ) return debouncedValue = false;
          return true;
        }
        if( previousValueMul100 > THRESHOLD_H ) return debouncedValue = true;
        return false;
      };
    }
  );

  //以前の値がfalseで新しい値がtrueか? を返すクロージャ
  auto isRaisedF(
    [](){
      bool previousValue ( false );
      return [=]( const bool CURRENT_VALUE ) mutable -> bool{
        const bool RESULT ( ! previousValue && CURRENT_VALUE );
        previousValue = CURRENT_VALUE;
        return RESULT;      
      };
    }
  );

  //以前の値がtrueで新しい値がfalseか? を返すクロージャ
  auto isDroppedF(
    [](){
      bool previousValue ( false );
      return [=]( const bool CURRENT_VALUE ) mutable -> bool{
        const bool RESULT ( previousValue && !CURRENT_VALUE );
        previousValue = CURRENT_VALUE;
        return RESULT;    
      };
    }
  );

}

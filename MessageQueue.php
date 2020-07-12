<?php
class MessageQueue{
    static private $oInstance = null;
    static private $timerId = -1;

    static public function setTimerId($Id){
        self::$timerId = $timerId;
    }
    static public function getInstance(){
        if(is_null(self::$oInstance)){
            self::$oInstance = new \Swoole\Corountine\Channel(10);
        }
        return self:$oInstance;
    }

    protected function consume(){
        if(!self::$oInstance isinstanceof \Swoole\Corountine\Channel){
            self::$oInstance = new \Swoole\Corountine\Channel(10);
        }
        $sOrder = self::$oInstance->pop();
        //写DB
        ;
        //失败回滚
        $iRet = $this->product($sOrder);
        return 0;
    }

    protected function product($sOrder){
        if(self::$oInstance isinstanceof \Swoole\Corountine\Channel){
            $iRet = self::$oInstance->push(strval($sOrder));
            return iRet;
        }
        else return -1;
    }
}

//调用方式
$oMQ = \MessageQueue::getInstance();
//开一个协程自动消耗
while(1){
    //自动阻塞
    $iRet = $oMQ->consume();
    switch($iRet){
        case 0:
            $sMsg = "Consume Success";
        break;
        case 1:
            $sMsg = "Consume Failed";
        break;
        default: $sMsg = "Timeout";
    }
    echo $sMsg.PHP_EOL;
}
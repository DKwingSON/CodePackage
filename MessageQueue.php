<?php
class MessageQueue{
    static private $oInstance = null;
    static private $timerId = -1;

    static public function setTimerId($Id){
        self::$timerId = $timerId;
    }

    //获取对象
    static public function getInstance(){
        if(is_null(self::$oInstance)){
            self::$oInstance = new \Swoole\Corountine\Channel(10);
        }
        return self::$oInstance;
    }

    //消息队列消耗一个对象;
    protected function consume(){
        if(!self::$oInstance instanceof \Swoole\Corountine\Channel){
            self::$oInstance = new \Swoole\Corountine\Channel(10);
        }
        $sOrder = self::$oInstance->pop();
        //TODO 解码sOrder
        $m_oDBProxy = new \DBConn();
        $m_configDB = $this->m_oDBProxy->getCfg();
        //TODO 写DB, INSERT
        $sql = "";
        $iRet = $m_oDBProxy->oConnDB->exec_update($sql);
        //失败回滚
        if($iRet != 0){
            $iRet = $this->product($sOrder);
            return $iRet;
        }
        else return 0;
    }

    protected function product($sOrder){
        if(self::$oInstance instanceof \Swoole\Corountine\Channel){
            $iRet = self::$oInstance->push(strval($sOrder));
            return iRet;
        }
        else return -1;
    }
}

//调用方式
//开一个协程自动消耗
go(function(){
    while(1){
        //自动阻塞
        $iRet = \MessageQueue::getInstance()->consume();
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
});

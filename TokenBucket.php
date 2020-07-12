<?php
/** 单机多进程令牌桶
 * @author drakeywang
 */

// Table 使用共享内存来保存数据，在创建子进程前，务必要执行 Table->create() ；
// Server 中使用 Table，Table->create() 必须在 Server->start() 前执行。
class TrafficBucket{
    private static $m_iMaxToken = null;
    private $m_oRedis = null;
    private $m_randKey = "";
    private $m_dtTodayBegin = null;
    private $m_dtTodayEnd = null;
    private $m_dtCurrnet = null;
    private $m_sTBName;

    //构造 获取
    public function __construct($maxToken){
        //Step1 获取Redis链接
        $this->m_oRedis = \RedisPool::getInstance()->getConnection('REDIS_CONN_INFO');

        //Step2 配置参数
        $this->m_sTBName = "TrafficBucket".$this->m_dtTodayBegin;
        $this->m_dtTodayBegin = strtotime(date('Y-m-d'),time());
        $this->m_dtTodayEnd =  $this->m_dtTodayBegin + 86400;
        $this->m_dtCurrent = time();
        if(is_null(\TrafficBucket::$m_iMaxToken) ){
            //第一次设置maxToken有效
            \TrafficBucket::$m_iMaxToken = $maxToken;
        }
    }

    public function __destrcut(){
        \RedisPool::getInstance()->free('REDIS_CONN_INFO', $this->m_oRedis);
    }

    /**
     * 加入新Token
     * @param int $num 加入的令牌数量，默认1
     * @return int 加入状态码， 0成功
     */
    public function add($num = 1){
        //有必要， 构造桶
        // echo $this->m_sTBName.PHP_EOL;
        if($this->m_oRedis->exists("__TrafficBucket__") == 0){
            $sLock = $this->m_sTBName."LOCK";
            $iLockStatus = $this->m_oRedis->set($sLock, $this->m_randKey, array('nx', 'ex' => 300));
            while(is_null($iLockStatus)){
                echo "Wait for resource...";
            }
            $this->m_oRedis->rPush($this->m_sTBName, ...range(1, intval(\TrafficBucket::$m_iMaxToken)));
            $this->m_oRedis->set("__TrafficBucket__", 1, array('nx', 'ex' => $this->m_dtTodayEnd - $this->m_dtCurrent));
            if($this->oRedis->get($sLock) == $this->m_randKey){
                $this->m_oRedis->del($sLock);
            }
        }
        $iRemainToken = min($num, intval(\TrafficBucket::$m_iMaxToken) - intval($this->getTokenNum()));
        if($iRemainToken > 1)
            return $this->m_oRedis->rPush($this->m_sTBName, ...range(1, $num));
        else if($iRemainToken == 1) 
            return $this->m_oRedis->rPush($this->m_sTBName, 1);
    }

    /**
     * 获得一个令牌
     * @return bool true获取成功
     */
    public function sub(){
        return $this->m_oRedis->lPop($this->m_sTBName)?true:false;
    }

    public function getTokenNum(){
        return $this->m_oRedis->lLen($this->m_sTBName);
    }
}
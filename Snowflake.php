<?php
/**
 * ID 生成策略
 * 毫秒级时间41位+机器ID 10位+毫秒内序列12位。
 * 0           41     51     64
+-----------+------+------+
|time       |pc    |inc   |
+-----------+------+------+
 *  前41bits是以微秒为单位的timestamp。
 *  接着10bits是事先配置好的机器ID。
 *  最后12bits是累加计数器。
 *  macheine id(10bits)标明最多只能有1024台机器同时产生ID，sequence number(12bits)也标明1台机器1ms中最多产生4096个ID，
 *
 * auth: zhouyuan
 */
class idwork
{
    /**
     * 生成限定长度随机字符串
     * @param length 输出字符串长度
     */
    function randomkeys($length)   
    {   
        $pattern = '1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLOMNOPQRSTUVWXYZ';
        $key = "";
        for($i=0; $i<$length; $i++){   
            $key =  $key.$pattern[rand(0,61)];    //生成php随机数   
            // sleep(0.1);
        }   
        return $key.microtime(true);   
    }

    
}

class testWork{
    private $privateInput;
    public function __construct($input)
    {
        $this->input = $input;
        $this->privateInput = intval($input) + 1;
    }
    
    public function output(){
        if(isset($this->input)){
            echo $this->input.PHP_EOL;
        }
        else echo "Undefined".PHP_EOL;
    }
}

class Snowflake
{
    const EPOCH = 1543223810238;    // 起始时间戳，毫秒

    const SEQUENCE_BITS = 12;   //序号部分12位
    const SEQUENCE_MAX = -1 ^ (-1 << self::SEQUENCE_BITS);  // 序号最大值

    const WORKER_BITS = 10; // 节点部分10位
    const WORKER_MAX = -1 ^ (-1 << self::WORKER_BITS);  // 节点最大数值

    const TIME_SHIFT = self::WORKER_BITS + self::SEQUENCE_BITS; // 时间戳部分左偏移量
    const WORKER_SHIFT = self::SEQUENCE_BITS;   // 节点部分左偏移量

    protected $timestamp;   // 上次ID生成时间戳
    protected $workerId;    // 节点ID
    protected $sequence;    // 序号
    protected $lock;        // Swoole 互斥锁

    public function __construct($workerId)
    {
        if ($workerId < 0 || $workerId > self::WORKER_MAX) {
            trigger_error("Worker ID 超出范围");
            exit(0);
        }

        $this->timestamp = 0;
        $this->workerId = $workerId;
        $this->sequence = 0;
    }

    /**
     * 生成ID
     * @return int
     */
    public function getId()
    {
        $now = $this->now();
        if ($this->timestamp == $now) {
            $this->sequence++;

            if ($this->sequence > self::SEQUENCE_MAX) {
                // 当前毫秒内生成的序号已经超出最大范围，等待下一毫秒重新生成
                while ($now <= $this->timestamp) {
                    $now = $this->now();
                }
            }
        } else {
            $this->sequence = 0;
        }

        $this->timestamp = $now;    // 更新ID生时间戳

        $id = (($now - self::EPOCH) << self::TIME_SHIFT) | ($this->workerId << self::WORKER_SHIFT) | $this->sequence;

        return $id;
    }

    /**
     * 获取当前毫秒
     * @return string
     */
    public function now()
    {
        return sprintf("%.0f", microtime(true) * 1000);
    }

}

$randID = new idwork(array("wordId" => 5));
$snowflake = new Snowflake(5);
$cnt = 0;
for($i = 0; $i < 10; $i++){
    $start1 = microtime(true);
    for($j = 0; $j < 10000; $j++){
        $id = $snowflake->getId();
    }
    $end1 = microtime(true);
    // echo "10000 ID for snowflake costs ".strval($end1-$start1)."s".PHP_EOL;
    $cnt += intval(10000/($end1-$start1));
    // echo "Estimate ".intval(10000/($end1-$start1))." ID/S".PHP_EOL;
}
echo "Snowflake Average: ".strval($cnt / 10)." ID/S".PHP_EOL;
// echo "Estimate ".intval(10000/($end1-$start1))." ID/S".PHP_EOL;

$cnt = 0;
for($i = 0; $i < 10; $i++){
    $start2 = microtime(true);
    for($j = 0; $j < 10000; $j++){
        $id = $randID->randomkeys(6);
    }
    $end2 = microtime(true);
    // echo "10000 ID for AMS Serial costs ".strval($end2-$start2)."s".PHP_EOL;
    $cnt += intval(10000/($end2-$start2));
    // echo "Estimate ".intval(10000/($end2-$start2))." ID/S".PHP_EOL;
}
echo "AMSSerial Average: ".strval($cnt / 10)." ID/S".PHP_EOL;

// $oTest = new testWork(123);
// $oTest->output();
// echo $oTest->input.PHP_EOL;
// echo $oTest->privateInput.PHP_EOL;
?>
--TEST--
InspectorOperand getNumber variable
--FILE--
<?php
use Inspector\InspectorFunction;
use Inspector\InspectorOpline;
use Inspector\InspectorBreakPoint;
use Inspector\InspectorFrame;

$function = function() {
	$a = sprintf("hello");
};

$inspector = 
	new InspectorFunction($function);

$opline = $inspector->getOpline(2);

$result = $opline->getOperand(InspectorOpline::RESULT);

if ($result->isVariable() || $result->isTemporaryVariable() &&
    $result->getNumber() == 0) {
	echo "OK";
}
?>
--EXPECT--
OK

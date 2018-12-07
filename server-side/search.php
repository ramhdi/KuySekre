<?php
$conn = mysqli_connect("localhost", "id6933861_ramhdi", "platipus") or die("Error connecting to database: ".mysqli_error($conn));
mysqli_select_db($conn, "id6933861_ramhdi_db") or die("Error selecting database: ".mysqli_error($conn));

$uid = $_GET['uid']; 
$min_length = 8;

if(strlen($uid) >= $uid){ // if query length is more or equal minimum length then 
	$uid = htmlspecialchars($uid); 

	// changes characters used in html to their equivalents, for example: < to &gt; 
	$uid = mysqli_real_escape_string($conn, $uid);

	// makes sure nobody uses SQL injection
	$raw_results = mysqli_query($conn, "SELECT * FROM kuysekre
		WHERE uid = '".$uid."' ") or die("Error getting data from database: ".mysqli_error($conn));

	if(mysqli_num_rows($raw_results) > 0){ // if one or more rows are returned do following

		while($results = mysqli_fetch_array($raw_results)){
		// $results = mysql_fetch_array($raw_results) puts data from database into array, while it's valid it does the loop
			$state = $results['state'];
			if ($state == 0)
			{
				$newstate = 1;
				$check = "masuk";
			} else {
				$newstate = 0;
				$check = "keluar";
			}

			mysqli_query($conn, "UPDATE kuysekre set state = ".$newstate." WHERE id = ".$results['id']) or die("Error updating database: ".mysqli_error($conn));
			echo $results['nim']." ".$check;
		}

	}
	else{ // if there is no matching rows do following
		echo "UID tidak ada";
	}

}
else{ // if query length is less than minimum
	echo "Minimum length is ".$min_length;
}
?>
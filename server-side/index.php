<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<title>KuySekre beta</title>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
</head>
<body>
	<?php
	$conn = mysqli_connect("localhost", "id6933861_ramhdi", "platipus") or die("Error connecting to database: ".mysqli_error($conn));
	mysqli_select_db($conn, "id6933861_ramhdi_db") or die("Error selecting database: ".mysqli_error($conn));

	$select_checkin = mysqli_query($conn, "SELECT nim, nama FROM kuysekre WHERE state = 1") or die("Error selecting data: ".mysqli_error($conn));

	echo "<h3>Daftar orang hadir di sekre:</h3>";
	if (mysqli_num_rows($select_checkin) > 0) {
    // output data of each row
		$i = 1;
		while($results = mysqli_fetch_array($select_checkin)) {
			echo $i." ".$results["nim"]." ".$results["nama"]."<br>"; $i++;
		}
	} else {
		echo "Sekre kosong.";
	}
	mysqli_close($conn);
	?>
</body>
</html>
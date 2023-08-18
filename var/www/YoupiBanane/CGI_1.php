<?php
$method = $_SERVER['REQUEST_METHOD'];
if ($method == "POST") {
  echo "Status: 200 OK\r\n";
  echo "Content-Type: text/html; charset=utf-8\r\n\r\n";
  
  $stdin = fopen('php://stdin', 'r');
  while(!feof($stdin)) $TXT .= fgets($stdin, 1024);
  fclose($stdin);
  
  $uploadDir = "./var/www/YoupiBanane/upload_post/";
  
  // Generate a unique file name using uniqid() function
  $uniqueFilename = uniqid() . '.png';
  $filePath = $uploadDir . $uniqueFilename;
  
  $fp = fopen($filePath, 'w');
  fwrite($fp, $TXT); 
  fclose($fp); 
  echo "파일이 업로드되었습니다. 저장된 파일명: " . $uniqueFilename;
    
} else {
  echo "Status: 501 INVALID_METHOD\r\n";
  echo "Content-Type: text/html; charset=utf-8\r\n\r\n";
  echo "<html>";
  echo "<head>";
  echo "<style>";
  echo "table {";
  echo "    border-collapse: collapse;";
  echo "    width: 80%;";
  echo "    margin: 0 auto;";
  echo "    text-align: center;";
  echo "}";
  echo "th, td {";
  echo "    border: 1px solid #dddddd;";
  echo "    padding: 8px;";
  echo "}";
  echo "th {";
  echo "    background-color: #f2f2f2;";
  echo "}";
  echo "</style>";
  echo "</head>";
  echo "<body>";
  echo "<h2>지원하지 않는 메소드입니다.</h2>";
  echo "</body>";
  echo "</html>\r\n\r\n";
}
?>
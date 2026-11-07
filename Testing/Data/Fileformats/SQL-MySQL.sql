-- MySQL / MariaDB syntax highlighting sample

SELECT
	`Customer
	 Name`,
	_utf8mb4' café ',
	42 AS `Answer`
FROM `customers`
WHERE `Status` = 'Active';

# A MySQL hash comment ends at this newline even when it ends with a backslash\
SELECT 1 AS value;

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*M!100100 SET @@GLOBAL.gtid_slave_pos='0-1-2' */;

SELECT
	`Customer Name`,
	JSON_EXTRACT(`details`, '$.customer.name') AS customer_name
FROM `events`;

-- Backslash escapes are valid in MySQL strings
SELECT 'it\'s a MySQL string', 'C:\\path';

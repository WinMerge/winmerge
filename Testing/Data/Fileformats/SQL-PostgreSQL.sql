-- PostgreSQL syntax highlighting sample

SELECT
	"Customer
	 Name",
	E'it\'s a multiline escape string
with a newline',
	ARRAY [1, /* array element comment */ 2, 3] AS values;

SELECT
	ARRAY[[1, 2], [3, 4]] AS matrix,
	"Customer Name" AS quoted_name
FROM "Customers"
WHERE "Status" = 'Active';

/* PostgreSQL uses regular block comments.
   /*! this must remain a comment, not executable SQL */ */
SELECT 1::integer AS value;

-- PostgreSQL operators containing # are not comments
SELECT json_data #>> '{customer,name}' AS customer_name
FROM events;

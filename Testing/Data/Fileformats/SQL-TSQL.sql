-- SQL Server / T-SQL syntax highlighting sample

SELECT
	[Customer
	 Name],
	'C:\',
	42 AS [Answer]
FROM [dbo].[Customers]
WHERE [Status] = 'Active';

-- Temporary table names and bracket-delimited identifiers
CREATE TABLE #CustomerNames
(
	[Customer Name] nvarchar(100)
);

INSERT INTO #CustomerNames ([Customer Name])
VALUES (N'Contoso');

SELECT [Customer Name]
FROM #CustomerNames;

/* A regular T-SQL block comment
   containing [brackets], 'quotes', and numbers 123. */
SELECT 1 AS [Still SQL];

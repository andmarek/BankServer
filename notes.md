# Supported Commands
## Transactions
TRANS <acct1> <amount1> <acct2> <amount2> ...

If transaction successful, write to file:
<requestID> OK TIME <starttime> <endtime>

If unsuccessful:
<requestID> ISF <acctid> TIME <starttime> <endtime>

## Balance Check
CHECK <accountid>

Output to file: <requestID> BAL <balance> TIME <starttime> <endtime>


# End server
END


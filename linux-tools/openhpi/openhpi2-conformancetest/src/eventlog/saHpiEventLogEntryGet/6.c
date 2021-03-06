/*
 * (C) Copyright IBM Corp. 2004, 2005
 * Copyright (c) 2005, Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307 USA.
 *
 * Author(s):
 *      Carl McAdams <carlmc@us.ibm.com>
 *      Xiaowei Yang <xiaowei.yang@intel.com>
 *      Qun Li <qun.li@intel.com>
 *
 * Spec:        HPI-B.01.01
 * Function:    saHpiEventLogEntryGet
 * Description:   
 *   Clear the event log. Call saHpiEventLogEntryGet to get an entry.
 *   saHpiEventLogEntryGet() returns SA_ERR_HPI_NOT_PRESENT.
 * Line:        P50-4:P50-5
 */
#include <stdio.h>
#include "saf_test.h"

int Test_Resource(SaHpiSessionIdT session, SaHpiResourceIdT resource)
{
	SaErrorT status;
	int retval = SAF_TEST_UNKNOWN;
	SaHpiEventLogEntryIdT PrevLogEntry;
	SaHpiEventLogEntryIdT NextLogEntry;
	SaHpiEventLogEntryT LogEntry;
	SaHpiBoolT EnableState;

	//
	// Save Event Log State.
	//
	status = saHpiEventLogStateGet(session, resource, &EnableState);
	if (status != SA_OK) {
		// Retrieving the event log failed
		e_print(saHpiEventLogStateGet, SA_OK, status);
		return SAF_TEST_UNRESOLVED;
	}
	//
	// Set EventLogState to false
	//
	status = saHpiEventLogStateSet(session, resource, SAHPI_FALSE);
	if (status != SA_OK) {
		// Setting event log  state failed
		e_print(saHpiEventLogStateSet, SA_OK, status);
		return SAF_TEST_UNRESOLVED;
	}
	//
	//  Clear the event log.
	// 
	status = saHpiEventLogClear(session, resource);
	if (status != SA_OK) {
		// Clearing the event log failed
		e_print(saHpiEventLogClear, SA_OK, status);
		retval = SAF_TEST_UNRESOLVED;
	} else {
		//
		//  Call saHpiEventLogEntryGet.
		//
		status = saHpiEventLogEntryGet(session,
					       resource,
					       SAHPI_OLDEST_ENTRY,
					       &PrevLogEntry,
					       &NextLogEntry,
					       &LogEntry, NULL, NULL);
		if (status == SA_ERR_HPI_NOT_PRESENT)
			retval = SAF_TEST_PASS;
		else {
			e_print(saHpiEventLogEntryGet, SA_ERR_HPI_NOT_PRESENT,
				status);
			retval = SAF_TEST_FAIL;
		}
	}

	//
	// Restore EventLog State
	//
	status = saHpiEventLogStateSet(session, resource, EnableState);
	if (status != SA_OK) {
		e_print(saHpiEventLogStateSet, SA_OK, status);
	}

	return (retval);
}

int Resource_Test(SaHpiSessionIdT session,
		  SaHpiRptEntryT rpt_entry, callback2_t func)
{
	int retval = SAF_TEST_UNKNOWN;

	if (rpt_entry.ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)
		retval = Test_Resource(session, rpt_entry.ResourceId);
	else			// This resource does not support Event logs
		retval = SAF_TEST_NOTSUPPORT;

	return (retval);
}

int Test_Domain(SaHpiSessionIdT session)
{
	int retval = SAF_TEST_UNKNOWN;

	// On each domain, test the domain event log.
	retval = Test_Resource(session, SAHPI_UNSPECIFIED_RESOURCE_ID);

	return (retval);
}

/**********************************************************
*   Main Function
*      takes no arguments
*      
*       returns: SAF_TEST_PASS when successfull
*                SAF_TEST_FAIL when an unexpected error occurs
*************************************************************/
int main(int argc, char **argv)
{
	int retval = SAF_TEST_UNKNOWN;

	retval = process_all_domains(Resource_Test, NULL, Test_Domain);

	return (retval);
}

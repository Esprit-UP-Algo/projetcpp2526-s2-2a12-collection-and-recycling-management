-- Migration: Add RFID UID column to EMPLOYE table
-- Date: 2026-04-27
-- Description: Adds RFID_UID field to support RFID card assignment for attendance system

-- Add RFID_UID column to EMPLOYE table
ALTER TABLE EMPLOYE
ADD RFID_UID VARCHAR(20) NULL;

-- Optional: Create an index for faster RFID lookups
CREATE INDEX idx_employe_rfid_uid ON EMPLOYE(RFID_UID);

-- Note: The column is nullable to allow employees without RFID cards initially
-- Maximum length 20 supports UID format like "2B:84:19:06" (11 chars) comfortably

-- To verify the change:
-- SELECT ID_EMPLOYE, NOM, RFID_UID FROM EMPLOYE;
-- GO

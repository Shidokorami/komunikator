SELECT name FROM sqlite_master WHERE type='table';

CREATE TABLE GROUPCHATS(
    chat_id INTEGER PRIMARY KEY,
    name TEXT NOT NULL

);

CREATE TABLE FRIENDS(
    username TEXT NOT NULL

);

CREATE TABLE MESSAGES(
    mess_id INTEGER PRIMARY KEY AUTOINCREMENT,
    chat_id INTEGER NOT NULL,
    content TEXT NOT NULL,
    sender_name TEXT NOT NULL,
    FOREIGN KEY (chat_id) REFERENCES GROUPCHATS(chat_id)

);

INSERT INTO GROUPCHATS(chat_id, name)
VALUES (1, 'Fajny czat'),
       (2, 'Super czat');

INSERT INTO MESSAGES(chat_id, content, sender_name)
VALUES (1, 'Wiadomosc 1', 'Kolega A'),
       (2, 'Wiadomosc 2', 'Kolega B');

SELECT * FROM GROUPCHATS;
SELECT * FROM MESSAGES;
SELECT * FROM FRIENDS;
DELETE FROM GROUPCHATS;
DELETE FROM MESSAGES;
DELETE FROM FRIENDS;




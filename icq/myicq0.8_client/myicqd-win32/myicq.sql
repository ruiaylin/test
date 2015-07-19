create table basic_tbl (
	uin INT UNSIGNED AUTO_INCREMENT,
	passwd CHAR(16) NOT NULL,
	pic TINYINT UNSIGNED NOT NULL,
	nick CHAR(16) NOT NULL,
	age TINYINT UNSIGNED NOT NULL,
	gender TINYINT UNSIGNED NOT NULL,
	country CHAR(16) NOT NULL,
	province CHAR(16) NOT NULL,
	city CHAR(20) NOT NULL,
	email CHAR(30) NOT NULL,
	auth TINYINT UNSIGNED NOT NULL,
	PRIMARY KEY(uin),
	INDEX(nick), INDEX(email)
)
AUTO_INCREMENT=1000;

CREATE TABLE ext_tbl (
	uin INT UNSIGNED PRIMARY KEY,
	address VARCHAR(64) NOT NULL,
	zipcode VARCHAR(16) NOT NULL,
	tel VARCHAR(32) NOT NULL,
	name VARCHAR(32) NOT NULL,
	blood TINYINT UNSIGNED NOT NULL,
	college VARCHAR(64) NOT NULL,
	profession VARCHAR(16) NOT NULL,
	homepage VARCHAR(64) NOT NULL,
	intro TINYTEXT NOT NULL
);

CREATE TABLE friend_tbl (
	uin1 INT UNSIGNED NOT NULL,
	uin2 INT UNSIGNED NOT NULL,
	INDEX(uin1), INDEX(uin2),
	UNIQUE(uin1, uin2)
);

CREATE TABLE message_tbl (
	dst INT UNSIGNED NOT NULL,
	src INT UNSIGNED NOT NULL,
	type TINYINT UNSIGNED NOT NULL,
	time INT UNSIGNED NOT NULL,
	msg TINYTEXT NOT NULL,
	INDEX(dst)
);

CREATE TABLE broadmsg_tbl (
	dst INT UNSIGNED NOT NULL,
	id INT UNSIGNED NOT NULL,
	INDEX(dst)
);

CREATE TABLE broadmsg_content_tbl (
	id INT UNSIGNED AUTO_INCREMENT,
	src INT UNSIGNED NOT NULL,
	type TINYINT UNSIGNED NOT NULL,
	time INT UNSIGNED NOT NULL,
	expire INT UNSIGNED NOT NULL,
	msg TINYTEXT NOT NULL,
	PRIMARY KEY(id)
);
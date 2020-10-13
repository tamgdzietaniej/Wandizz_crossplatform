CREATE TABLE `users` (
 `id` int(11) NOT NULL AUTO_INCREMENT,
 `oauth_provider` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
 `oauth_uid` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
 `username` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
 `fname` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
 `lname` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
 `locale` varchar(10) COLLATE utf8_unicode_ci NOT NULL,
 `oauth_token` text COLLATE utf8_unicode_ci NOT NULL,
 `oauth_secret` text COLLATE utf8_unicode_ci NOT NULL,
 `picture` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
 `created` datetime NOT NULL,
 `modified` datetime NOT NULL,
 PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci

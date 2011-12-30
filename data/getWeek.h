int getWeek(time_t current) {
	const struct tm * timeinfo = localtime(&current);
	if (timeinfo->tm_wday == 0) {
		return (timeinfo->tm_yday - timeinfo->tm_wday+4)/7;
	}
	return (timeinfo->tm_yday - timeinfo->tm_wday+4)/7 + 1;
}


<?php
//start session
session_start();

include_once("config.php");
include_once("inc/twitteroauth.php");
?>
<!DOCTYPE html>
<html lang="en">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<title>Twitter OAuth Login Using PHP | W3tweaks</title>
    <style type="text/css">
	.wrapper{width:600px; margin-left:auto;margin-right:auto;}
	.welcome_txt{
		margin: 20px;
		background-color: #EBEBEB;
		padding: 10px;
		border: #D6D6D6 solid 1px;
		-moz-border-radius:5px;
		-webkit-border-radius:5px;
		border-radius:5px;
	}
	.tweet_box{
		margin: 20px;
		background-color: #FFF0DD;
		padding: 10px;
		border: #F7CFCF solid 1px;
		-moz-border-radius:5px;
		-webkit-border-radius:5px;
		border-radius:5px;
	}
	.tweet_box textarea{
		width: 500px;
		border: #F7CFCF solid 1px;
		-moz-border-radius:5px;
		-webkit-border-radius:5px;
		border-radius:5px;
	}
	.tweet_list{
		margin: 20px;
		padding:20px;
		background-color: #E2FFF9;
		border: #CBECCE solid 1px;
		-moz-border-radius:5px;
		-webkit-border-radius:5px;
		border-radius:5px;
	}
	.tweet_list ul{
		padding: 0px;
		font-family: verdana;
		font-size: 12px;
		color: #5C5C5C;
	}
	.tweet_list li{
		border-bottom: silver dashed 1px;
		list-style: none;
		padding: 5px;
	}
	</style>
</head>
<body>
<?php
	if(isset($_SESSION['status']) && $_SESSION['status'] == 'verified') 
	{
		//Retrive variables
		$screen_name 		= $_SESSION['request_vars']['screen_name'];
		$twitter_id			= $_SESSION['request_vars']['user_id'];
		$oauth_token 		= $_SESSION['request_vars']['oauth_token'];
		$oauth_token_secret = $_SESSION['request_vars']['oauth_token_secret'];
	
		//Show welcome message
		echo '<div class="welcome_txt">Welcome <strong>'.$screen_name.'</strong> (Twitter ID : '.$twitter_id.'). <a href="logout.php?logout">Logout</a>!</div>';
		$connection = new TwitterOAuth(CONSUMER_KEY, CONSUMER_SECRET, $oauth_token, $oauth_token_secret);
		
		//If user wants to tweet using form.
		if(isset($_POST["updateme"])) 
		{
			//Post text to twitter
			$my_update = $connection->post('statuses/update', array('status' => $_POST["updateme"]));
			die('<script type="text/javascript">window.top.location="index.php"</script>'); //redirect back to index.php
		}
		
		//show tweet form
		echo '<div class="tweet_box">';
		echo '<form method="post" action="index.php"><table width="200" border="0" cellpadding="3">';
		echo '<tr>';
		echo '<td><textarea name="updateme" cols="60" rows="4"></textarea></td>';
		echo '</tr>';
		echo '<tr>';
		echo '<td><input type="submit" value="Tweet" /></td>';
		echo '</tr></table></form>';
		echo '</div>';
		
		//Get latest tweets
		$my_tweets = $connection->get('statuses/user_timeline', array('screen_name' => $screen_name, 'count' => 5));
		
		echo '<div class="tweet_list"><strong>Latest Tweets : </strong>';
		echo '<ul>';
		foreach ($my_tweets  as $my_tweet) {
			echo '<li>'.$my_tweet->text.' <br />-<i>'.$my_tweet->created_at.'</i></li>';
		}
		echo '</ul></div>';
			
	}else{
		//Display login button
		echo '<a href="process.php"><img src="images/twitter.png"/></a>';
	}
?>  
</body>
</html>
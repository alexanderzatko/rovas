<?php

namespace Hybridauth\Provider;

use Hybridauth\Adapter\OAuth2;
use Hybridauth\Data;
use Hybridauth\Exception\UnexpectedApiResponseException;
use Hybridauth\User;

class Nabezky extends OAuth2
{

    protected $scope = 'read_prefs';

    protected $apiBaseUrl = 'https://nabezky.sk';
    
    protected $authorizeUrl = 'https://nabezky.sk/oauth2/authorize';
    
    protected $accessTokenUrl = 'https://nabezky.sk/oauth2/token';

    public function getUserProfile()
    {
        $response = $this->apiRequest('oauth2/userinfo');

        $data = new Data\Collection($response);
watchdog('library','OpenStreetMap userProfile ' . print_r($data,1));
		$sub = $data->get('sub');

        if (!$sub) {
            throw new UnexpectedApiResponseException('Provider API returned an unexpected response.');
        }

		$userProfile = new User\Profile();

		$userProfile->identifier = null != $data->get('sub') ? $data->get('sub') : null;
		$userProfile->displayName = null != $data->get('name') ? $data->get('name') : null;
		$userProfile->email = null != $data->get('email') ? $data->get('email') : null;
		$userProfile->profileURL = null != $data->get('sub') ? 'https://nabezky.sk/user/' . $data->get('sub') : null;
		$userProfile->emailVerified = null != $data->get('email_verified') ? $data->get('email_verified') : null;

watchdog('library','OpenStreetMap userProfile ' . print_r($userProfile,1));

		return $userProfile;		
    }
}

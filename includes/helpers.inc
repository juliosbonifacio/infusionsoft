<?php
/**
 * @file
 * helpers.inc.
 */

/**
 * Retrieves and stores the infusionsoft tracking code.
 */
function _infusionsoft_update_tracking_code() {
  $tracking_code = infusionsoft_send_request('WebTrackingService.getWebTrackingScriptTag');
  if (!empty($tracking_code)) {
    $matches = array();
    // Only store the url so we can be non protocol specific.
    preg_match('/(?<=src=")\S*(?=")/', $tracking_code, $matches);
    variable_set('infusionsoft_tracking_url', check_url($matches[0]));
  }
}

/**
 * The default data mapping.
 *
 * @param object $user
 *   The user object.
 *
 * @param array $options
 *   An array of options to use when updating a contact.
 *
 * @return array
 *   The data mapping.
 */
function _infusionsoft_map_default($user, $options) {
  return array(
    'Username' => $user->name,
    'Email' => $user->mail,
  );
}

/**
 * Returns a list of Contact Maps for use on the map select field.
 *
 * @param array $required_options
 *   An array of required options. (optional)
 *
 * @return array
 *   An array of options to be selected in the actions select form.
 */
function _infusionsoft_contact_map_select($required_options = array()) {
  // Build list values.
  $options_array = array();
  $map_info = infusionsoft_contact_maps();
  foreach ($map_info as $key => $map) {
    if (!empty($map['callback'])) {
      // If there are required options, lets make sure the map has them.
      if (!empty($required_options)) {
        // This map has no options, we can skip this map.
        if (empty($map['options']) || !is_array($map['options'])) {
          continue;
        }

        // Initialize if the option is allowed.
        $is_allowed = TRUE;

        foreach ($required_options as $required_option) {
          if (!in_array($required_option, $map['options'])) {
            $is_allowed = FALSE;
          }
        }
        // Do not include in the list if the map is not allowed due to not
        // having the right options.
        if ($is_allowed == FALSE) {
          continue;
        }
      }
      $options_array[$key]  = $map['title'] . ': ' . $map['description'];
    }
  }
  return $options_array;
}

/**
 * Returns a list of Contact Groups for use on the contact group select field.
 *
 * @return array
 *   An array of options to be selected in the actions select form.
 */
function _infusionsoft_get_contact_group_select() {
  $options_array = array();

  // List each category group id.
  $categories = _infusionsoft_get_contact_group_categories();
  if (!empty($categories)) {
    $return_fields = array('Id', 'GroupName', 'GroupCategoryId', 'GroupDescription');
    foreach ($categories as $category) {
      $query = array('GroupCategoryId' => $category['Id']);
      $group_names = infusionsoft_query_all('ContactGroup', $query, $return_fields);
      if (!empty($group_names)) {
        foreach ($group_names as $group_name) {
          $options_array[$group_name['Id']]  = $category['CategoryName'] . ': ' . $group_name['GroupName'];
        }
      }
    }
  }
  return $options_array;
}

/**
 * Returns a list of Follow up sequences for use on the infusionsoft follow up select field.
 *
 * @return array
 *   An array of options to be selected in the actions select form.
 */
function _infusionsoft_get_follow_up_select() {
  $options_array = array();

  // List each category group id.
  $query = array('Name' => '%');
  $return_fields = array('Id', 'Name', 'Status');
  $campaigns = infusionsoft_query_all('Campaign', $query, $return_fields);
  if (!empty($campaigns)) {
    foreach ($campaigns as $campaign) {
      $label = $campaign['Name'];
      if ($campaign['Status'] != 'Active') {
        $label .= ' (' . $campaign['Status'] . ')';
      }
      $options_array[$campaign['Id']]  = $label;
    }
  }
  return $options_array;
}

/**
 * Returns a list of Action Sets for use on the infusionsoft Action Set select field.
 *
 * @return array
 *   An array of options to be selected in the actions select form.
 */
function _infusionsoft_get_action_set_select() {
  $options_array = array();

  // List each category group id.
  $query = array('Id' => '%');
  $return_fields = array('Id', 'TemplateName');
  $action_sets = infusionsoft_query_all('ActionSequence', $query, $return_fields);
  if (!empty($action_sets)) {
    foreach ($action_sets as $action_set) {
      $options_array[$action_set['Id']]  = $action_set['Id'] . ': '. $action_set['TemplateName'];
    }
  }
  return $options_array;
}

/**
 * Returns a list of Lead Sources for use on the infusionsoft lead source select field.
 *
 *  @return array
 *     An array of options to be selected in the lead source select form.
 */
function _infusionsoft_get_lead_source_select() {
  $options_array = array();

  // List each category group id.
  $query = array('Id' => '%');
  $return_fields = array('Id', 'Name');
  $lead_sources = infusionsoft_query_all('LeadSource', $query, $return_fields);
  if (is_array($lead_sources)) {
    foreach ($lead_sources as $lead_source) {
      $options_array[$lead_source['Id']]  = $lead_source['Id'] . ': '. $lead_source['TemplateName'];
    }
  }
  else {
    drupal_set_message(t('Error, array not returned'));
  }
  return $options_array;
}

/**
 * List of all Contact Group Categories
 * Helper function for infusionsoft group selection
 *
 * @return array
 *   An array of contact group catagories in the format:
 *    array(
 *      array(
 *        'Id' => (int) some number,
 *        'CatagoryName' => 'Some Name',
 *        'CategoryDescription' => 'Some description'
 *      )
 *      ...
 *    );
 */
function _infusionsoft_get_contact_group_categories() {
  $return_fields = array('Id', 'CategoryName', 'CategoryDescription');
  $query = array('Id' => '%');
  return infusionsoft_query_all('ContactGroupCategory', $query, $return_fields);
}

/**
 * Helper function returns the first contact id in an array of contact ids
 *
 * @param int|array $contact_id
 *   A contact_id or list of contact_ids.
 *
 * @return int
 *   Returns the contact_id if it is not an array.
 */
function _infusionsoft_get_first_contact_id($contact_id = 0) {
  // If there are multiple contact ids in an array, use the first one.
  if (is_array($contact_id)) {
    $contact_id = $contact_id[0]['Id'];
  }
  return (int) $contact_id;
}

/**
 *  Cache a user contact id for a user.
 *
 * @param object $user
 *   drupal user object
 *
 * @return int|bool
 *   returns the first contact id for the matching the email address provided by the username.
 *   if there is no username, it returns false. Caches infusionsoft ids, for less hits to
 *   infusionsoft database.
 */
function _infusionsoft_user_cache($user) {
  // Return FALSE if a user is anonymous or does not exist.
  if ($user->uid < 1) {
    return FALSE;
  }

  // If we stored the infusionsoft id in the database, return this first.
  $query = db_result(db_query("SELECT infusionsoft_id FROM {infusionsoft} WHERE uid = '%d'", $user->uid));
  if (!empty($query)) {
    return (int)$query;
  }

  // If contact exists in infusionsoft, add to db and return id.
  $contact_id = infusionsoft_contactid_by_email($user->mail);
  if (!empty($contact_id)) {
    db_query("INSERT INTO {infusionsoft} (uid, infusionsoft_id) VALUES (%d, %d)", $user->uid, $contact_id);
    return $contact_id;
  }

  // Could not find user's contact id, return FALSE.
  return FALSE;
}

/**
 * Clear the infusionsoft user table
 */
function _infusionsoft_user_cache_clear() {
  db_query("TRUNCATE TABLE {infusionsoft}");
  _infusionsoft_devel('infusionsoft', 'infusionsoft user cache has been cleared');
}

/**
 * Test infusionsoft xmlrpc connection
 */
function _infusionsoft_test_connection() {
  $app_name = variable_get('infusionsoft_appname', '');
  $server = "https://$app_name.infusionsoft.com/api/xmlrpc";

  // DataService.echo is undocumented on the infusionsoft site, however it acts
  // as expected. Send a string, it is echoed back.
  $result = xmlrpc($server, 'DataService.echo', 'Infusionsoft has successfully connected.');
  return $result;
}

/**
 * Send developer information. Works with watchdog and devel.
 *
 * @param string $type
 *   The category to which this message belongs. Can be any string, but the
 *   general practice is to use the name of the module calling
 *   _infusionsoft_devel().
 *
 * @param string $message
 *   The message to store in the log. See t() for documentation on how
 *   $message and $variables interact. Keep $message translatable by not
 *   concatenating dynamic values into it!
 *
 * @param array $variables
 *   Array of variables to replace in the message on display or NULL if message
 *   is already translated or not possible to translate.
 *
 * @param int $severity
 *   The severity of the message, as per RFC 3164. Possible values are
 *   WATCHDOG_ERROR, WATCHDOG_WARNING, etc.
 *
 * @param string $link
 *   A link to associate with the message.
 */
function _infusionsoft_devel($type, $message, $variables = array(), $severity = WATCHDOG_NOTICE, $link = NULL) {
  if (variable_get('infusionsoft_watchdog', FALSE) == TRUE) {
    watchdog($type, $message, $variables, $severity, $link);
  }
  if (module_exists('devel') && variable_get('infusionsoft_devel', FALSE) == TRUE) {
    dpm(t($message, $variables));
  }
}
